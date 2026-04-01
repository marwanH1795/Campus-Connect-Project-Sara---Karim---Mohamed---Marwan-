#include "ChatController.h"
#include "Message.h"

ChatController::ChatController(std::shared_ptr<INetworkClient> net)
    : network(net)
{
    network->setMessageHandler([this](const std::string& json) {
        handleIncomingMessage(json);
    });
}

bool ChatController::connectUser(const std::string& username) {
    if (!InputValidator::isValidUsername(username)) {
        return false;
    }

    state.setCurrentUser(username);
    network->connectToServer(username);
    return true;
}

bool ChatController::sendPublicMessage(const std::string& content) {
    if (!InputValidator::isValidMessage(content)) {
        return false;
    }

    Message msg(MessageType::PublicMessage,
                state.getCurrentUser(),
                "",
                "",
                content,
                "");

    network->sendMessage(msg.toJson());
    return true;
}

void ChatController::handleIncomingMessage(const std::string& json) {
    Message msg = Message::fromJson(json);

    if (msg.getType() == MessageType::PublicMessage) {
        state.addPublicMessage(msg);
    }
}

ChatState& ChatController::getState() {
    return state;
}
