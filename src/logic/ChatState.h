#ifndef CHATSTATE_H
#define CHATSTATE_H

#include <string>
#include <vector>
<<<<<<< HEAD
=======
#include <map>
>>>>>>> origin/Marwan
#include "Message.h"

class ChatState {
private:
    std::string currentUser;
    std::vector<std::string> onlineUsers;
<<<<<<< HEAD
    std::vector<Message> publicMessages;
=======

    std::vector<Message> publicMessages;
    std::vector<Message> privateMessages;
>>>>>>> origin/Marwan

    std::vector<std::string> joinedGroups;
    std::vector<Message> groupMessages;

<<<<<<< HEAD
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
=======
    std::vector<std::string> publicTypingUsers;
    std::map<std::string, std::vector<std::string>> groupTypingUsers;
    std::map<std::string, bool> privateTypingUsers;

public:
    void setCurrentUser(const std::string& username);
    std::string getCurrentUser() const;

    void setOnlineUsers(const std::vector<std::string>& users);
    std::vector<std::string> getOnlineUsers() const;

    void addPublicMessage(const Message& message);
    std::vector<Message> getPublicMessages() const;

    void addPrivateMessage(const Message& message);
    std::vector<Message> getPrivateMessagesWithUser(const std::string& username) const;

>>>>>>> origin/Marwan
    void joinGroup(const std::string& groupId);
    bool isInGroup(const std::string& groupId) const;
    std::vector<std::string> getJoinedGroups() const;

    void addGroupMessage(const Message& message);
    std::vector<Message> getGroupMessagesForGroup(const std::string& groupId) const;
<<<<<<< HEAD
=======

    void setPublicUserTyping(const std::string& username, bool isTyping);
    std::vector<std::string> getPublicTypingUsers() const;

    void setGroupUserTyping(const std::string& groupId,
                            const std::string& username,
                            bool isTyping);
    std::vector<std::string> getGroupTypingUsers(const std::string& groupId) const;

    void setPrivateUserTyping(const std::string& username, bool isTyping);
    bool isPrivateUserTyping(const std::string& username) const;
>>>>>>> origin/Marwan
};

#endif
