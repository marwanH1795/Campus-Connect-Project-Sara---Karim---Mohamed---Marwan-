#include "Message.h"

Message::Message()
    : type(MessageType::Unknown),
      sender(""),
      target(""),
      groupId(""),
      content(""),
      timestamp("")
{
}

Message::Message(MessageType type,
                 const std::string& sender,
                 const std::string& target,
                 const std::string& groupId,
                 const std::string& content,
                 const std::string& timestamp)
    : type(type),
      sender(sender),
      target(target),
      groupId(groupId),
      content(content),
      timestamp(timestamp)
{
}

MessageType Message::getType() const {
    return type;
}

std::string Message::getSender() const {
    return sender;
}

std::string Message::getTarget() const {
    return target;
}

std::string Message::getGroupId() const {
    return groupId;
}

std::string Message::getContent() const {
    return content;
}

std::string Message::getTimestamp() const {
    return timestamp;
}

void Message::setType(MessageType newType) {
    type = newType;
}

void Message::setSender(const std::string& newSender) {
    sender = newSender;
}

void Message::setTarget(const std::string& newTarget) {
    target = newTarget;
}

void Message::setGroupId(const std::string& newGroupId) {
    groupId = newGroupId;
}

void Message::setContent(const std::string& newContent) {
    content = newContent;
}

void Message::setTimestamp(const std::string& newTimestamp) {
    timestamp = newTimestamp;
}

std::string Message::messageTypeToString(MessageType type) {
    switch (type) {
        case MessageType::Connect: return "connect";
        case MessageType::ConnectAck: return "connect_ack";
        case MessageType::PublicMessage: return "public_message";
        case MessageType::PrivateMessage: return "private_message";
        case MessageType::CreateGroup: return "create_group";
        case MessageType::GroupMessage: return "group_message";
        case MessageType::UserList: return "user_list";
        case MessageType::Error: return "error";
        case MessageType::History: return "history";
        case MessageType::AIRequest: return "ai_request";
        case MessageType::AIResponse: return "ai_response";
        default: return "unknown";
    }
}

MessageType Message::stringToMessageType(const std::string& typeString) {
    if (typeString == "connect") return MessageType::Connect;
    if (typeString == "connect_ack") return MessageType::ConnectAck;
    if (typeString == "public_message") return MessageType::PublicMessage;
    if (typeString == "private_message") return MessageType::PrivateMessage;
    if (typeString == "create_group") return MessageType::CreateGroup;
    if (typeString == "group_message") return MessageType::GroupMessage;
    if (typeString == "user_list") return MessageType::UserList;
    if (typeString == "error") return MessageType::Error;
    if (typeString == "history") return MessageType::History;
    if (typeString == "ai_request") return MessageType::AIRequest;
    if (typeString == "ai_response") return MessageType::AIResponse;

    return MessageType::Unknown;
}

std::string Message::toJson() const {
    return "{"
           "\"type\":\"" + messageTypeToString(type) + "\","
           "\"sender\":\"" + sender + "\","
           "\"target\":\"" + target + "\","
           "\"groupId\":\"" + groupId + "\","
           "\"content\":\"" + content + "\","
           "\"timestamp\":\"" + timestamp + "\""
           "}";
}

Message Message::fromJson(const std::string& json) {
    (void)json;
    return Message();
}
