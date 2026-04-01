#include "ChatState.h"

void ChatState::setCurrentUser(const std::string& username) {
    currentUser = username;
}

std::string ChatState::getCurrentUser() const {
    return currentUser;
}

void ChatState::setOnlineUsers(const std::vector<std::string>& users) {
    onlineUsers = users;
}

std::vector<std::string> ChatState::getOnlineUsers() const {
    return onlineUsers;
}

void ChatState::addPublicMessage(const Message& message) {
    publicMessages.push_back(message);
}

std::vector<Message> ChatState::getPublicMessages() const {
    return publicMessages;
}
