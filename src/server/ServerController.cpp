#include "ServerController.h"
#include "ClientSession.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

#include <iostream>

void ServerController::onClientConnected(std::shared_ptr<ClientSession> client) {
    state.addClient(client);
    std::cout << "New client connected" << std::endl;
}

void ServerController::onClientDisconnected(std::shared_ptr<ClientSession> client) {
    state.removeClient(client);
    std::cout << "Client disconnected" << std::endl;
}

void ServerController::onMessageReceived(std::shared_ptr<ClientSession> client, const std::string& rawJson) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(rawJson), &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        std::cout << "Invalid JSON received: " << rawJson << std::endl;
        return;
    }

    QJsonObject obj = doc.object();

    QString type = obj.value("type").toString();
    QString groupId = obj.value("groupId").toString();

    std::cout << "Received JSON: " << rawJson << std::endl;

    if (type == "public_message") {
        broadcastToAll(rawJson);
        return;
    }

    if (type == "create_group") {
        if (!groupId.isEmpty()) {
            state.createGroup(groupId.toStdString(), client);
            client->send(rawJson);
            std::cout << "Client added to created group: "
                      << groupId.toStdString() << std::endl;
        }
        return;
    }

    if (type == "join_group") {
        if (!groupId.isEmpty()) {
            state.joinGroup(groupId.toStdString(), client);
            client->send(rawJson);
            std::cout << "Client joined group: "
                      << groupId.toStdString() << std::endl;
        }
        return;
    }

    if (type == "group_message") {
        std::string group = groupId.toStdString();

        if (!group.empty() && state.isMember(group, client)) {
            sendToGroup(group, rawJson);
            std::cout << "Broadcasted group message to: "
                      << group << std::endl;
        } else {
            std::cout << "Rejected group message for non-member group: "
                      << group << std::endl;
        }

        return;
    }

    std::cout << "Unknown message type: " << type.toStdString() << std::endl;
}

void ServerController::broadcastToAll(const std::string& rawJson) {
    for (auto& client : state.getAllClients()) {
        if (client) {
            client->send(rawJson);
        }
    }
}

void ServerController::sendToGroup(const std::string& groupId, const std::string& rawJson) {
    for (auto& client : state.getGroupMembers(groupId)) {
        if (client) {
            client->send(rawJson);
        }
    }
}
