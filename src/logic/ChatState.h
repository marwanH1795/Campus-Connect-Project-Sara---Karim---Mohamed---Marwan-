#ifndef CHATSTATE_H
#define CHATSTATE_H

#include <string>
#include <vector>
#include "Message.h"

class ChatState {
private:
    std::string currentUser;
    std::vector<std::string> onlineUsers;
    std::vector<Message> publicMessages;

    std::vector<std::string> joinedGroups;
    std::vector<Message> groupMessages;

public:
    // user
    void setCurrentUser(const std::string& username);
    std::string getCurrentUser() const;

    // online users
    void setOnlineUsers(const std::vector<std::string>& users);
    std::vector<std::string> getOnlineUsers() const;

    // public chat
    void addPublicMessage(const Message& message);
    std::vector<Message> getPublicMessages() const;

    // groups
    void joinGroup(const std::string& groupId);
    bool isInGroup(const std::string& groupId) const;
    std::vector<std::string> getJoinedGroups() const;

    void addGroupMessage(const Message& message);
    std::vector<Message> getGroupMessagesForGroup(const std::string& groupId) const;
};

#endif
