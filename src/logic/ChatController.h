#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#include <memory>
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

    ChatState& getState();
};

#endif
