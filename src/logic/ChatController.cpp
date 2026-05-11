#include "ChatController.h"
#include "Message.h"

#include <QTime>
#include <QDateTime>
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QStringList>
#include <QDebug>

#include <sstream>

static std::string currentTimestamp() {
    return QTime::currentTime().toString("hh:mm").toStdString();
}

static std::vector<std::string> splitUsers(const std::string& text) {
    std::vector<std::string> users;
    std::stringstream ss(text);
    std::string item;

    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            users.push_back(item);
        }
    }

    return users;
}

ChatController::ChatController(std::shared_ptr<INetworkClient> net)
    : network(net)
{
    network->setMessageHandler([this](const std::string& json) {
        handleIncomingMessage(json);
    });

    network->setBinaryHandler([this](const QByteArray& data) {
        handleIncomingBinary(data);
    });
}

bool ChatController::connectUser(const std::string& username) {
    return connectUser(username, "127.0.0.1", 12345);
}

bool ChatController::connectUser(const std::string& username,
                                 const std::string& host,
                                 unsigned short port) {
    if (!InputValidator::isValidUsername(username)) {
        return false;
    }

    if (host.empty() || port == 0) {
        return false;
    }

    state.setCurrentUser(username);
    network->connectToServer(username, host, port);
    return true;
}

bool ChatController::sendPublicMessage(const std::string& content) {
    if (!InputValidator::isValidMessage(content)) {
        return false;
    }

    sendTypingStatus(false);

    Message msg(MessageType::PublicMessage,
                state.getCurrentUser(),
                "",
                "",
                content,
                currentTimestamp());

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendPrivateMessage(const std::string& target,
                                        const std::string& content) {
    if (target.empty() || !InputValidator::isValidMessage(content)) {
        return false;
    }

    sendPrivateTypingStatus(target, false);

    Message msg(MessageType::PrivateMessage,
                state.getCurrentUser(),
                target,
                "",
                content,
                currentTimestamp());

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
                currentTimestamp());

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
                currentTimestamp());

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendGroupMessage(const std::string& groupId,
                                      const std::string& content) {
    if (groupId.empty() || !InputValidator::isValidMessage(content)) {
        return false;
    }

    if (!state.isInGroup(groupId)) {
        return false;
    }

    sendGroupTypingStatus(groupId, false);

    Message msg(MessageType::GroupMessage,
                state.getCurrentUser(),
                "",
                groupId,
                content,
                currentTimestamp());

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendTypingStatus(bool isTyping) {
    if (state.getCurrentUser().empty()) {
        return false;
    }

    Message msg(MessageType::Typing,
                state.getCurrentUser(),
                "",
                "",
                isTyping ? "typing" : "stopped",
                currentTimestamp());

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendPrivateTypingStatus(const std::string& target,
                                             bool isTyping) {
    if (state.getCurrentUser().empty() || target.empty()) {
        return false;
    }

    Message msg(MessageType::Typing,
                state.getCurrentUser(),
                target,
                "",
                isTyping ? "typing" : "stopped",
                currentTimestamp());

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendGroupTypingStatus(const std::string& groupId,
                                           bool isTyping) {
    if (state.getCurrentUser().empty() || groupId.empty()) {
        return false;
    }

    Message msg(MessageType::Typing,
                state.getCurrentUser(),
                "",
                groupId,
                isTyping ? "typing" : "stopped",
                currentTimestamp());

    network->sendMessage(msg.toJson());
    return true;
}

bool ChatController::sendPublicAttachment(const QString& filePath,
                                          const QString& fileType) {
    return sendAttachmentFile(filePath, fileType, "", "");
}

bool ChatController::sendPrivateAttachment(const std::string& target,
                                           const QString& filePath,
                                           const QString& fileType) {
    if (target.empty()) {
        return false;
    }

    return sendAttachmentFile(filePath, fileType, target, "");
}

bool ChatController::sendGroupAttachment(const std::string& groupId,
                                         const QString& filePath,
                                         const QString& fileType) {
    if (groupId.empty()) {
        return false;
    }

    if (!state.isInGroup(groupId)) {
        return false;
    }

    return sendAttachmentFile(filePath, fileType, "", groupId);
}

bool ChatController::sendAttachmentFile(const QString& filePath,
                                        const QString& fileType,
                                        const std::string& target,
                                        const std::string& groupId) {
    QFileInfo info(filePath);

    if (!info.exists() || info.size() <= 0) {
        return false;
    }

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QString fileId =
        QString::fromStdString(state.getCurrentUser()) +
        "_" +
        QString::number(QDateTime::currentMSecsSinceEpoch());

    QString meta =
        fileId + "|" +
        info.fileName() + "|" +
        fileType + "|" +
        QString::number(info.size());

    Message startMsg(MessageType::AttachmentStart,
                     state.getCurrentUser(),
                     target,
                     groupId,
                     meta.toStdString(),
                     currentTimestamp());

    network->sendMessage(startMsg.toJson());

    const qint64 chunkSize = 32 * 1024;

    while (!file.atEnd()) {
        QByteArray chunk = file.read(chunkSize);

        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);

        QByteArray idBytes = fileId.toUtf8();

        stream << quint16(idBytes.size());
        packet.append(idBytes);
        packet.append(chunk);

        network->sendBinary(packet);
    }

    file.close();

    Message endMsg(MessageType::AttachmentEnd,
                   state.getCurrentUser(),
                   target,
                   groupId,
                   fileId.toStdString(),
                   currentTimestamp());

    network->sendMessage(endMsg.toJson());

    return true;
}

QString ChatController::createUniqueReceivedFilePath(const QString& fileName) const {
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath() + "/CampusConnectData";
    }

    QDir dir(baseDir + "/received_attachments");

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString safeFileName = QFileInfo(fileName).fileName();
    QString outputPath = dir.absoluteFilePath(safeFileName);

    QFileInfo info(outputPath);
    int counter = 1;

    while (QFileInfo::exists(outputPath)) {
        QString suffix = info.suffix();
        QString baseName = info.completeBaseName();

        if (suffix.isEmpty()) {
            outputPath = dir.absoluteFilePath(baseName + "_" + QString::number(counter));
        } else {
            outputPath = dir.absoluteFilePath(
                baseName + "_" + QString::number(counter) + "." + suffix
            );
        }

        counter++;
    }

    return outputPath;
}

void ChatController::handleIncomingBinary(const QByteArray& data) {
    if (data.size() < 2) {
        return;
    }

    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    quint16 idSize = 0;
    stream >> idSize;

    if (idSize == 0 || data.size() < 2 + idSize) {
        return;
    }

    QByteArray idBytes = data.mid(2, idSize);
    QString fileId = QString::fromUtf8(idBytes);

    QByteArray chunk = data.mid(2 + idSize);

    auto it = incomingAttachments.find(fileId.toStdString());

    if (it == incomingAttachments.end()) {
        qDebug() << "Received binary chunk for unknown fileId:" << fileId;
        return;
    }

    if (it->second.file && it->second.file->isOpen()) {
        it->second.file->write(chunk);
    }
}

void ChatController::finishIncomingAttachment(const Message& msg) {
    QString fileId = QString::fromStdString(msg.getContent());

    auto it = incomingAttachments.find(fileId.toStdString());

    if (it == incomingAttachments.end()) {
        return;
    }

    IncomingAttachment attachment = it->second;

    if (attachment.file && attachment.file->isOpen()) {
        attachment.file->flush();
        attachment.file->close();
    }

    incomingAttachments.erase(it);

    QString renderedContent;

    if (attachment.fileType.startsWith("audio")) {
        renderedContent =
            "VOICE_FILE|" +
            attachment.fileName +
            "|" +
            attachment.filePath;
    }
    else if (attachment.fileType.startsWith("image")) {
        renderedContent =
            "IMAGE_FILE|" +
            attachment.fileName +
            "|" +
            attachment.filePath;
    }
    else if (attachment.fileType.startsWith("video")) {
        renderedContent =
            "VIDEO_FILE|" +
            attachment.fileName +
            "|" +
            attachment.filePath;
    }
    else {
        renderedContent =
            "FILE_ATTACHMENT|" +
            attachment.fileName +
            "|" +
            attachment.filePath;
    }

    Message displayed;

    if (!attachment.groupId.empty()) {
        displayed = Message(MessageType::GroupMessage,
                            attachment.sender,
                            "",
                            attachment.groupId,
                            renderedContent.toStdString(),
                            currentTimestamp());

        state.setGroupUserTyping(attachment.groupId, attachment.sender, false);

        if (state.isInGroup(attachment.groupId)) {
            state.addGroupMessage(displayed);
        }
    }
    else if (!attachment.target.empty()) {
        displayed = Message(MessageType::PrivateMessage,
                            attachment.sender,
                            attachment.target,
                            "",
                            renderedContent.toStdString(),
                            currentTimestamp());

        state.setPrivateUserTyping(attachment.sender, false);
        state.addPrivateMessage(displayed);
    }
    else {
        displayed = Message(MessageType::PublicMessage,
                            attachment.sender,
                            "",
                            "",
                            renderedContent.toStdString(),
                            currentTimestamp());

        state.setPublicUserTyping(attachment.sender, false);
        state.addPublicMessage(displayed);
    }
}

void ChatController::handleIncomingMessage(const std::string& json) {
    Message msg = Message::fromJson(json);

    if (msg.getType() == MessageType::PublicMessage) {
        state.setPublicUserTyping(msg.getSender(), false);
        state.addPublicMessage(msg);
    }
    else if (msg.getType() == MessageType::PrivateMessage) {
        state.setPrivateUserTyping(msg.getSender(), false);
        state.addPrivateMessage(msg);
    }
    else if (msg.getType() == MessageType::CreateGroup ||
             msg.getType() == MessageType::JoinGroup) {
        state.joinGroup(msg.getGroupId());
    }
    else if (msg.getType() == MessageType::GroupMessage) {
        state.setGroupUserTyping(msg.getGroupId(), msg.getSender(), false);

        if (state.isInGroup(msg.getGroupId())) {
            state.addGroupMessage(msg);
        }
    }
    else if (msg.getType() == MessageType::Typing) {
        bool isTyping = (msg.getContent() == "typing");

        if (!msg.getGroupId().empty()) {
            state.setGroupUserTyping(msg.getGroupId(), msg.getSender(), isTyping);
        }
        else if (!msg.getTarget().empty()) {
            state.setPrivateUserTyping(msg.getSender(), isTyping);
        }
        else {
            state.setPublicUserTyping(msg.getSender(), isTyping);
        }
    }
    else if (msg.getType() == MessageType::UserList) {
        state.setOnlineUsers(splitUsers(msg.getContent()));
    }
    else if (msg.getType() == MessageType::AttachmentStart) {
        QString content = QString::fromStdString(msg.getContent());
        QStringList parts = content.split("|");

        if (parts.size() < 4) {
            return;
        }

        QString fileId = parts[0];
        QString fileName = parts[1];
        QString fileType = parts[2];

        QString outputPath = createUniqueReceivedFilePath(fileName);

        std::shared_ptr<QFile> file = std::make_shared<QFile>(outputPath);

        if (!file->open(QIODevice::WriteOnly)) {
            return;
        }

        IncomingAttachment attachment;
        attachment.fileId = fileId;
        attachment.fileName = fileName;
        attachment.fileType = fileType;
        attachment.filePath = outputPath;
        attachment.sender = msg.getSender();
        attachment.target = msg.getTarget();
        attachment.groupId = msg.getGroupId();
        attachment.file = file;

        incomingAttachments[fileId.toStdString()] = attachment;
    }
    else if (msg.getType() == MessageType::AttachmentEnd) {
        finishIncomingAttachment(msg);
    }
}

ChatState& ChatController::getState() {
    return state;
}

std::string ChatController::getCurrentUsername() const {
    return state.getCurrentUser();
}
