#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

class ClientSession;

class ServerState {
public:
    void addClient(std::shared_ptr<ClientSession> client);
    void removeClient(std::shared_ptr<ClientSession> client);

    void createGroup(const std::string& groupId, std::shared_ptr<ClientSession> client);
    void joinGroup(const std::string& groupId, std::shared_ptr<ClientSession> client);

    bool isMember(const std::string& groupId, std::shared_ptr<ClientSession> client);

    std::unordered_set<std::shared_ptr<ClientSession>> getGroupMembers(const std::string& groupId);

    std::unordered_set<std::shared_ptr<ClientSession>> getAllClients();

private:
    std::unordered_set<std::shared_ptr<ClientSession>> clients;
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<ClientSession>>> groups;
};
