#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

enum class MessageType {
    Connect,
    ConnectAck,
    PublicMessage,
    PrivateMessage,
    CreateGroup,
    GroupMessage,
    UserList,
    Error,
    History,
    AIRequest,
    AIResponse,
    Unknown
};

class Message {
private:
    MessageType type;
    std::string sender;
    std::string target;
    std::string groupId;
    std::string content;
    std::string timestamp;

public:
    Message();
    Message(MessageType type,
            const std::string& sender,
            const std::string& target = "",
            const std::string& groupId = "",
            const std::string& content = "",
            const std::string& timestamp = "");

    MessageType getType() const;
    std::string getSender() const;
    std::string getTarget() const;
    std::string getGroupId() const;
    std::string getContent() const;
    std::string getTimestamp() const;

    void setType(MessageType newType);
    void setSender(const std::string& newSender);
    void setTarget(const std::string& newTarget);
    void setGroupId(const std::string& newGroupId);
    void setContent(const std::string& newContent);
    void setTimestamp(const std::string& newTimestamp);

    static std::string messageTypeToString(MessageType type);
    static MessageType stringToMessageType(const std::string& typeString);

    std::string toJson() const;
    static Message fromJson(const std::string& json);
};

#endif
