#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#include <memory>
<<<<<<< HEAD
=======
#include <string>

>>>>>>> origin/Marwan
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
<<<<<<< HEAD
    bool sendPublicMessage(const std::string& content);
=======

    bool sendPublicMessage(const std::string& content);
    bool sendPrivateMessage(const std::string& target, const std::string& content);
>>>>>>> origin/Marwan

    bool createGroup(const std::string& groupId);
    bool joinGroup(const std::string& groupId);
    bool sendGroupMessage(const std::string& groupId, const std::string& content);

<<<<<<< HEAD
    ChatState& getState();
=======
    bool sendTypingStatus(bool isTyping);
    bool sendPrivateTypingStatus(const std::string& target, bool isTyping);
    bool sendGroupTypingStatus(const std::string& groupId, bool isTyping);

    ChatState& getState();
    std::string getCurrentUsername() const;
>>>>>>> origin/Marwan
};

#endif
