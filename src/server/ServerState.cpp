#include "ServerState.h"

void ServerState::addClient(std::shared_ptr<ClientSession> client) {
    clients.insert(client);
}

void ServerState::removeClient(std::shared_ptr<ClientSession> client) {
    clients.erase(client);

    for (auto& [group, members] : groups) {
        members.erase(client);
    }
}

void ServerState::createGroup(const std::string& groupId, std::shared_ptr<ClientSession> client) {
    groups[groupId].insert(client);
}

void ServerState::joinGroup(const std::string& groupId, std::shared_ptr<ClientSession> client) {
    groups[groupId].insert(client);
}

bool ServerState::isMember(const std::string& groupId, std::shared_ptr<ClientSession> client) {
    return groups[groupId].count(client) > 0;
}

std::unordered_set<std::shared_ptr<ClientSession>> ServerState::getGroupMembers(const std::string& groupId) {
    return groups[groupId];
}

std::unordered_set<std::shared_ptr<ClientSession>> ServerState::getAllClients() {
    return clients;
}
