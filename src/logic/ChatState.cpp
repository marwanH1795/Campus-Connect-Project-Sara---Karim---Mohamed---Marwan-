#include "ChatState.h"

#include <algorithm>

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

void ChatState::joinGroup(const std::string& groupId) {
    if (groupId.empty()) {
        return;
    }

    if (std::find(joinedGroups.begin(), joinedGroups.end(), groupId) == joinedGroups.end()) {
        joinedGroups.push_back(groupId);
    }
}

bool ChatState::isInGroup(const std::string& groupId) const {
    return std::find(joinedGroups.begin(), joinedGroups.end(), groupId) != joinedGroups.end();
}

std::vector<std::string> ChatState::getJoinedGroups() const {
    return joinedGroups;
}

void ChatState::addGroupMessage(const Message& message) {
    if (message.getGroupId().empty()) {
        return;
    }

    groupMessages.push_back(message);
}

std::vector<Message> ChatState::getGroupMessagesForGroup(const std::string& groupId) const {
    std::vector<Message> filtered;

    for (const auto& msg : groupMessages) {
        if (msg.getGroupId() == groupId) {
            filtered.push_back(msg);
        }
    }

    return filtered;
}
