#include "ChatController.h"
#include "Message.h"

<<<<<<< HEAD
=======
#include <QTime>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QString>
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

static bool isVoicePayload(const std::string& content) {
    return content.rfind("VOICE_DATA|", 0) == 0;
}

static QString saveIncomingVoiceFile(const std::string& payload) {
    QString text = QString::fromStdString(payload);

    QString prefix = "VOICE_DATA|";

    if (!text.startsWith(prefix)) {
        return "";
    }

    QString rest = text.mid(prefix.length());
    int separatorIndex = rest.indexOf('|');

    if (separatorIndex <= 0) {
        return "";
    }

    QString fileName = rest.left(separatorIndex);
    QString base64Audio = rest.mid(separatorIndex + 1);

    QByteArray audioData = QByteArray::fromBase64(base64Audio.toLatin1());

    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath() + "/CampusConnectData";
    }

    QDir dir(baseDir + "/received_voice");

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString safeFileName = QFileInfo(fileName).fileName();
    QString outputPath = dir.absoluteFilePath(safeFileName);

    int counter = 1;
    QFileInfo info(outputPath);

    while (QFileInfo::exists(outputPath)) {
        QString baseName = info.completeBaseName();
        QString suffix = info.suffix();

        outputPath = dir.absoluteFilePath(
            baseName + "_" + QString::number(counter) + "." + suffix
        );

        counter++;
    }

    QFile out(outputPath);

    if (!out.open(QIODevice::WriteOnly)) {
        return "";
    }

    out.write(audioData);
    out.close();

    return outputPath;
}

static Message convertVoicePayloadIfNeeded(const Message& original) {
    if (!isVoicePayload(original.getContent())) {
        return original;
    }

    QString savedPath = saveIncomingVoiceFile(original.getContent());

    if (savedPath.isEmpty()) {
        Message failed = original;
        failed.setContent("🎤 Voice message could not be saved");
        return failed;
    }

    QString text = QString::fromStdString(original.getContent());
    QString rest = text.mid(QString("VOICE_DATA|").length());
    int separatorIndex = rest.indexOf('|');

    QString fileName = "voice message";

    if (separatorIndex > 0) {
        fileName = rest.left(separatorIndex);
    }

    Message converted = original;
    converted.setContent(
        ("VOICE_FILE|" + fileName + "|" + savedPath).toStdString()
    );

    return converted;
}

>>>>>>> origin/Marwan
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

<<<<<<< HEAD
=======
    sendTypingStatus(false);

>>>>>>> origin/Marwan
    Message msg(MessageType::PublicMessage,
                state.getCurrentUser(),
                "",
                "",
                content,
<<<<<<< HEAD
                "");
=======
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
>>>>>>> origin/Marwan

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
<<<<<<< HEAD
                "");
=======
                currentTimestamp());
>>>>>>> origin/Marwan

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
<<<<<<< HEAD
                "");
=======
                currentTimestamp());
>>>>>>> origin/Marwan

    network->sendMessage(msg.toJson());
    return true;
}

<<<<<<< HEAD
bool ChatController::sendGroupMessage(const std::string& groupId, const std::string& content) {
=======
bool ChatController::sendGroupMessage(const std::string& groupId,
                                      const std::string& content) {
>>>>>>> origin/Marwan
    if (groupId.empty() || !InputValidator::isValidMessage(content)) {
        return false;
    }

    if (!state.isInGroup(groupId)) {
        return false;
    }

<<<<<<< HEAD
=======
    sendGroupTypingStatus(groupId, false);

>>>>>>> origin/Marwan
    Message msg(MessageType::GroupMessage,
                state.getCurrentUser(),
                "",
                groupId,
                content,
<<<<<<< HEAD
                "");
=======
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

bool ChatController::sendPrivateTypingStatus(const std::string& target, bool isTyping) {
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
>>>>>>> origin/Marwan

    network->sendMessage(msg.toJson());
    return true;
}

void ChatController::handleIncomingMessage(const std::string& json) {
    Message msg = Message::fromJson(json);

    if (msg.getType() == MessageType::PublicMessage) {
<<<<<<< HEAD
        state.addPublicMessage(msg);
    }
=======
        msg = convertVoicePayloadIfNeeded(msg);

        state.setPublicUserTyping(msg.getSender(), false);
        state.addPublicMessage(msg);
    }
    else if (msg.getType() == MessageType::PrivateMessage) {
        msg = convertVoicePayloadIfNeeded(msg);

        state.setPrivateUserTyping(msg.getSender(), false);
        state.addPrivateMessage(msg);
    }
>>>>>>> origin/Marwan
    else if (msg.getType() == MessageType::CreateGroup ||
             msg.getType() == MessageType::JoinGroup) {
        state.joinGroup(msg.getGroupId());
    }
    else if (msg.getType() == MessageType::GroupMessage) {
<<<<<<< HEAD
=======
        msg = convertVoicePayloadIfNeeded(msg);

        state.setGroupUserTyping(msg.getGroupId(), msg.getSender(), false);

>>>>>>> origin/Marwan
        if (state.isInGroup(msg.getGroupId())) {
            state.addGroupMessage(msg);
        }
    }
<<<<<<< HEAD
=======
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
>>>>>>> origin/Marwan
}

ChatState& ChatController::getState() {
    return state;
}
<<<<<<< HEAD
=======

std::string ChatController::getCurrentUsername() const {
    return state.getCurrentUser();
}
>>>>>>> origin/Marwan
