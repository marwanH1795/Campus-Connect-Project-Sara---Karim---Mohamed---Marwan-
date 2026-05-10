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

bool ChatController::createGroup(const std::string& groupId) {
    if (groupId.empty()) {
        return false;
    }

    Message msg(MessageType::CreateGroup,
                state.getCurrentUser(),
                "",
                groupId,
                "",
                "");

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::joinGroup(const std::string& groupId) {
    if (groupId.empty()) {
        return false;
    }

    Message msg(MessageType::JoinGroup,
                state.getCurrentUser(),
                "",
                groupId,
                "",
                "");

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendGroupMessage(const std::string& groupId, const std::string& content) {
    if (groupId.empty() || !InputValidator::isValidMessage(content)) {
        return false;
    }

    if (!state.isInGroup(groupId)) {
        return false;
    }

    Message msg(MessageType::GroupMessage,
                state.getCurrentUser(),
                "",
                groupId,
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
    else if (msg.getType() == MessageType::CreateGroup ||
             msg.getType() == MessageType::JoinGroup) {
        state.joinGroup(msg.getGroupId());
    }
    else if (msg.getType() == MessageType::GroupMessage) {
        if (state.isInGroup(msg.getGroupId())) {
            state.addGroupMessage(msg);
        }
    }
}

ChatState& ChatController::getState() {
    return state;
}
