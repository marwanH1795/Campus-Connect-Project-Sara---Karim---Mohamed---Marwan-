#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#include <memory>
#include <string>
#include <map>

#include <QByteArray>
#include <QString>
#include <QFile>

#include "ChatState.h"
#include "InputValidator.h"
#include "../network/INetworkClient.h"

struct IncomingAttachment {
    QString fileId;
    QString fileName;
    QString fileType;
    QString filePath;

    std::string sender;
    std::string target;
    std::string groupId;

    std::shared_ptr<QFile> file;
};

class ChatController {
private:
    ChatState state;
    std::shared_ptr<INetworkClient> network;

    std::map<std::string, IncomingAttachment> incomingAttachments;

    void handleIncomingMessage(const std::string& json);
    void handleIncomingBinary(const QByteArray& data);

    bool sendAttachmentFile(const QString& filePath,
                            const QString& fileType,
                            const std::string& target,
                            const std::string& groupId);

    QString createUniqueReceivedFilePath(const QString& fileName) const;
    void finishIncomingAttachment(const Message& msg);

public:
    ChatController(std::shared_ptr<INetworkClient> net);

    bool connectUser(const std::string& username);

    bool sendPublicMessage(const std::string& content);
    bool sendPrivateMessage(const std::string& target, const std::string& content);

    bool createGroup(const std::string& groupId);
    bool joinGroup(const std::string& groupId);
    bool sendGroupMessage(const std::string& groupId, const std::string& content);

    bool sendTypingStatus(bool isTyping);
    bool sendPrivateTypingStatus(const std::string& target, bool isTyping);
    bool sendGroupTypingStatus(const std::string& groupId, bool isTyping);

    bool sendPublicAttachment(const QString& filePath, const QString& fileType);
    bool sendPrivateAttachment(const std::string& target,
                               const QString& filePath,
                               const QString& fileType);
    bool sendGroupAttachment(const std::string& groupId,
                             const QString& filePath,
                             const QString& fileType);

    ChatState& getState();
    std::string getCurrentUsername() const;
};

#endif
