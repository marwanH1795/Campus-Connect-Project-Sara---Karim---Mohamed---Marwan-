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

void ChatState::addPrivateMessage(const Message& message) {
    privateMessages.push_back(message);
}

std::vector<Message> ChatState::getPrivateMessagesWithUser(const std::string& username) const {
    std::vector<Message> filtered;

    for (const auto& msg : privateMessages) {
        bool sentByUser = msg.getSender() == username && msg.getTarget() == currentUser;
        bool sentToUser = msg.getSender() == currentUser && msg.getTarget() == username;

        if (sentByUser || sentToUser) {
            filtered.push_back(msg);
        }
    }

    return filtered;
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

void ChatState::setPublicUserTyping(const std::string& username, bool isTyping) {
    if (username.empty() || username == currentUser) {
        return;
    }

    auto it = std::find(publicTypingUsers.begin(), publicTypingUsers.end(), username);

    if (isTyping) {
        if (it == publicTypingUsers.end()) {
            publicTypingUsers.push_back(username);
        }
    } else {
        if (it != publicTypingUsers.end()) {
            publicTypingUsers.erase(it);
        }
    }
}

std::vector<std::string> ChatState::getPublicTypingUsers() const {
    return publicTypingUsers;
}

void ChatState::setGroupUserTyping(const std::string& groupId,
                                   const std::string& username,
                                   bool isTyping) {
    if (groupId.empty() || username.empty() || username == currentUser) {
        return;
    }

    auto& users = groupTypingUsers[groupId];
    auto it = std::find(users.begin(), users.end(), username);

    if (isTyping) {
        if (it == users.end()) {
            users.push_back(username);
        }
    } else {
        if (it != users.end()) {
            users.erase(it);
        }
    }
}

std::vector<std::string> ChatState::getGroupTypingUsers(const std::string& groupId) const {
    auto it = groupTypingUsers.find(groupId);

    if (it == groupTypingUsers.end()) {
        return {};
    }

    return it->second;
}

void ChatState::setPrivateUserTyping(const std::string& username, bool isTyping) {
    if (username.empty() || username == currentUser) {
        return;
    }

    privateTypingUsers[username] = isTyping;
}

bool ChatState::isPrivateUserTyping(const std::string& username) const {
    auto it = privateTypingUsers.find(username);

    if (it == privateTypingUsers.end()) {
        return false;
    }

    return it->second;
}
