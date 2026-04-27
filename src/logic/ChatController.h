#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#include <memory>
#include <string>

#include "ChatState.h"
#include "InputValidator.h"
#include "../network/INetworkClient.h"

class ChatController {
private:
    ChatState state;
    std::shared_ptr<INetworkClient> network;

    void handleIncomingMessage(const std::string& json);

public:
    ChatController(std::shared_ptr<INetworkClient> net);

    bool connectUser(const std::string& username);
    bool sendPublicMessage(const std::string& content);

    bool createGroup(const std::string& groupId);
    bool joinGroup(const std::string& groupId);
    bool sendGroupMessage(const std::string& groupId, const std::string& content);

    ChatState& getState();
    std::string getCurrentUsername() const;
};

#endif
