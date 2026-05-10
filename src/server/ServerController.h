#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <vector>
#include <memory>
#include <map>
#include <string>

class ClientSession;

class ServerController {
private:
    std::vector<std::shared_ptr<ClientSession>> clients;
    std::map<std::string, std::shared_ptr<ClientSession>> users;
    std::map<std::string, std::vector<std::shared_ptr<ClientSession>>> groups;

    std::vector<std::string> publicHistory;
    std::map<std::string, std::vector<std::string>> groupHistory;

    void loadPublicHistory();
    void savePublicMessage(const std::string& msg);

    void loadGroupHistory(const std::string& groupId);
    void saveGroupMessage(const std::string& groupId, const std::string& msg);

    void sendPublicHistory(std::shared_ptr<ClientSession> client);
    void sendGroupHistory(const std::string& groupId,
                          std::shared_ptr<ClientSession> client);

    void sendToGroup(const std::string& groupId, const std::string& msg);
    void addClientToGroup(const std::string& groupId,
                          std::shared_ptr<ClientSession> client);

    void broadcastUserList();

public:
    ServerController();

    void registerClient(std::shared_ptr<ClientSession> client);
    void removeClient(std::shared_ptr<ClientSession> client);

    void handleMessage(const std::string& msg,
                       std::shared_ptr<ClientSession> sender);

    void broadcast(const std::string& msg);
};

#endif
