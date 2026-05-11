#include "ServerController.h"
#include "ClientSession.h"
#include "../logic/Message.h"

#include <algorithm>
#include <fstream>
#include <iostream>

ServerController::ServerController() {
    loadPublicHistory();
}

void ServerController::loadPublicHistory() {
    std::ifstream file("public_history.txt");
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            publicHistory.push_back(line);
        }
    }
}

void ServerController::savePublicMessage(const std::string& msg) {
    std::ofstream file("public_history.txt", std::ios::app);
    file << msg << "\n";
}

void ServerController::loadGroupHistory(const std::string& groupId) {
    if (groupHistory.count(groupId)) {
        return;
    }

    std::ifstream file("group_" + groupId + ".txt");
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            groupHistory[groupId].push_back(line);
        }
    }
}

void ServerController::saveGroupMessage(const std::string& groupId,
                                        const std::string& msg) {
    std::ofstream file("group_" + groupId + ".txt", std::ios::app);
    file << msg << "\n";
}

void ServerController::registerClient(std::shared_ptr<ClientSession> client) {
    clients.push_back(client);
}

void ServerController::removeClient(std::shared_ptr<ClientSession> client) {
    clients.erase(
        std::remove(clients.begin(), clients.end(), client),
        clients.end()
    );

    if (!client->getUsername().empty()) {
        users.erase(client->getUsername());
        broadcastUserList();
    }

    for (auto& pair : groups) {
        auto& groupClients = pair.second;

        groupClients.erase(
            std::remove(groupClients.begin(), groupClients.end(), client),
            groupClients.end()
        );
    }
}

std::string ServerController::extractFileIdFromAttachmentStart(const std::string& content) const {
    std::size_t pos = content.find('|');

    if (pos == std::string::npos) {
        return "";
    }

    return content.substr(0, pos);
}

std::string ServerController::extractFileIdFromBinaryPacket(const char* data,
                                                            std::size_t size) const {
    if (size < 2) {
        return "";
    }

    unsigned char b0 = static_cast<unsigned char>(data[0]);
    unsigned char b1 = static_cast<unsigned char>(data[1]);

    std::uint16_t idSize =
        static_cast<std::uint16_t>((b0 << 8) | b1);

    if (idSize == 0 || size < 2 + idSize) {
        return "";
    }

    return std::string(data + 2, data + 2 + idSize);
}

void ServerController::handleMessage(const std::string& raw,
                                     std::shared_ptr<ClientSession> sender) {
    std::cout << "SERVER RECEIVED TEXT, size: " << raw.size() << std::endl;

    Message msg = Message::fromJson(raw);

    switch (msg.getType()) {

    case MessageType::Connect:
        sender->setUsername(msg.getSender());
        users[msg.getSender()] = sender;

        sender->send(raw);
        sendPublicHistory(sender);
        broadcastUserList();
        break;

    case MessageType::PublicMessage:
        publicHistory.push_back(raw);
        savePublicMessage(raw);
        broadcast(raw);
        break;

    case MessageType::PrivateMessage:
        if (users.count(msg.getTarget())) {
            users[msg.getTarget()]->send(raw);
        }

        sender->send(raw);
        break;

    case MessageType::Typing:
        if (!msg.getTarget().empty()) {
            if (users.count(msg.getTarget())) {
                users[msg.getTarget()]->send(raw);
            }

            sender->send(raw);
        }
        else if (!msg.getGroupId().empty()) {
            sendToGroup(msg.getGroupId(), raw);
        }
        else {
            broadcast(raw);
        }
        break;

    case MessageType::CreateGroup:
        addClientToGroup(msg.getGroupId(), sender);
        loadGroupHistory(msg.getGroupId());
        sender->send(raw);
        sendGroupHistory(msg.getGroupId(), sender);
        break;

    case MessageType::JoinGroup:
        addClientToGroup(msg.getGroupId(), sender);
        loadGroupHistory(msg.getGroupId());
        sender->send(raw);
        sendGroupHistory(msg.getGroupId(), sender);
        break;

    case MessageType::GroupMessage:
        groupHistory[msg.getGroupId()].push_back(raw);
        saveGroupMessage(msg.getGroupId(), raw);
        sendToGroup(msg.getGroupId(), raw);
        break;

    case MessageType::AttachmentStart: {
        std::string fileId = extractFileIdFromAttachmentStart(msg.getContent());

        if (!fileId.empty()) {
            AttachmentRoute route;
            route.sender = msg.getSender();
            route.target = msg.getTarget();
            route.groupId = msg.getGroupId();

            activeAttachmentRoutes[fileId] = route;
        }

        if (!msg.getTarget().empty()) {
            if (users.count(msg.getTarget())) {
                users[msg.getTarget()]->send(raw);
            }

            sender->send(raw);
        }
        else if (!msg.getGroupId().empty()) {
            sendToGroup(msg.getGroupId(), raw);
        }
        else {
            broadcast(raw);
        }

        break;
    }

    case MessageType::AttachmentEnd: {
        if (!msg.getTarget().empty()) {
            if (users.count(msg.getTarget())) {
                users[msg.getTarget()]->send(raw);
            }

            sender->send(raw);
        }
        else if (!msg.getGroupId().empty()) {
            sendToGroup(msg.getGroupId(), raw);
        }
        else {
            broadcast(raw);
        }

        activeAttachmentRoutes.erase(msg.getContent());
        break;
    }

    default:
        std::cout << "SERVER WARNING: Unknown message type" << std::endl;
        break;
    }
}

void ServerController::handleBinary(const char* data,
                                    std::size_t size,
                                    std::shared_ptr<ClientSession>) {
    std::string fileId = extractFileIdFromBinaryPacket(data, size);

    if (fileId.empty()) {
        std::cout << "SERVER WARNING: Binary packet without fileId" << std::endl;
        return;
    }

    routeBinaryByFileId(fileId, data, size);
}

void ServerController::routeBinaryByFileId(const std::string& fileId,
                                           const char* data,
                                           std::size_t size) {
    auto it = activeAttachmentRoutes.find(fileId);

    if (it == activeAttachmentRoutes.end()) {
        std::cout << "SERVER WARNING: Unknown attachment fileId: " << fileId << std::endl;
        return;
    }

    const AttachmentRoute& route = it->second;

    if (!route.target.empty()) {
        if (users.count(route.target)) {
            users[route.target]->sendBinary(data, size);
        }

        if (users.count(route.sender)) {
            users[route.sender]->sendBinary(data, size);
        }

        return;
    }

    if (!route.groupId.empty()) {
        sendBinaryToGroup(route.groupId, data, size);
        return;
    }

    for (auto& client : clients) {
        client->sendBinary(data, size);
    }
}

void ServerController::broadcast(const std::string& msg) {
    for (auto& client : clients) {
        client->send(msg);
    }
}

void ServerController::sendToGroup(const std::string& groupId,
                                   const std::string& msg) {
    if (!groups.count(groupId)) {
        return;
    }

    for (auto& client : groups[groupId]) {
        client->send(msg);
    }
}

void ServerController::sendBinaryToGroup(const std::string& groupId,
                                         const char* data,
                                         std::size_t size) {
    if (!groups.count(groupId)) {
        return;
    }

    for (auto& client : groups[groupId]) {
        client->sendBinary(data, size);
    }
}

void ServerController::addClientToGroup(const std::string& groupId,
                                        std::shared_ptr<ClientSession> client) {
    if (groupId.empty()) {
        return;
    }

    auto& groupClients = groups[groupId];

    if (std::find(groupClients.begin(), groupClients.end(), client) == groupClients.end()) {
        groupClients.push_back(client);
    }
}

void ServerController::sendPublicHistory(std::shared_ptr<ClientSession> client) {
    for (const auto& msg : publicHistory) {
        client->send(msg);
    }
}

void ServerController::sendGroupHistory(const std::string& groupId,
                                        std::shared_ptr<ClientSession> client) {
    for (const auto& msg : groupHistory[groupId]) {
        client->send(msg);
    }
}

void ServerController::broadcastUserList() {
    std::string content;

    for (const auto& pair : users) {
        if (!content.empty()) {
            content += ",";
        }

        content += pair.first;
    }

    Message msg(MessageType::UserList,
                "server",
                "",
                "",
                content,
                "");

    broadcast(msg.toJson());
}
