#pragma once

#include "ServerState.h"

#include <memory>
#include <string>

class ClientSession;

class ServerController {
public:
    void onClientConnected(std::shared_ptr<ClientSession> client);
    void onClientDisconnected(std::shared_ptr<ClientSession> client);
    void onMessageReceived(std::shared_ptr<ClientSession> client, const std::string& rawJson);

private:
    ServerState state;

    void broadcastToAll(const std::string& rawJson);
    void sendToGroup(const std::string& groupId, const std::string& rawJson);
};
