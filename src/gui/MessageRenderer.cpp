#include "MessageRenderer.h"

#include <QTime>
#include <QByteArray>

static bool isVoiceFileMessage(const QString& content) {
    return content.startsWith("VOICE_FILE|");
}

static QString voiceFileName(const QString& content) {
    QString rest = content.mid(QString("VOICE_FILE|").length());
    int separatorIndex = rest.indexOf('|');

    if (separatorIndex <= 0) {
        return "voice message";
    }

    return rest.left(separatorIndex).toHtmlEscaped();
}

static QString voiceFilePath(const QString& content) {
    QString rest = content.mid(QString("VOICE_FILE|").length());
    int separatorIndex = rest.indexOf('|');

    if (separatorIndex <= 0) {
        return "";
    }

    return rest.mid(separatorIndex + 1);
}

static QString renderVoiceContent(const QString& content) {
    QString name = voiceFileName(content);
    QString path = voiceFilePath(content);

    QString encodedPath = QString::fromLatin1(path.toUtf8().toBase64());

    return QString(
        "<a href='voice:%1' style='text-decoration:none;'>"
        "<span style='background-color:#ffffff; color:#111827; "
        "padding:7px 12px; border-radius:14px; font-weight:bold;'>"
        "▶ Play voice message"
        "</span>"
        "</a>"
        "<div style='font-size:10px; margin-top:8px; opacity:0.85;'>%2</div>"
    ).arg(encodedPath, name);
}

QString MessageRenderer::renderMessage(const Message& msg, const QString& currentUser)
{
    QString sender = QString::fromStdString(msg.getSender()).toHtmlEscaped();
    QString target = QString::fromStdString(msg.getTarget()).toHtmlEscaped();
    QString rawContent = QString::fromStdString(msg.getContent());
    QString content = rawContent.toHtmlEscaped();
    QString time = QString::fromStdString(msg.getTimestamp()).toHtmlEscaped();

    if (time.isEmpty()) {
        time = QTime::currentTime().toString("hh:mm");
    }

    bool isMe = (sender.toLower() == currentUser);

    QString align = isMe ? "right" : "left";
    QString bubbleColor = isMe ? "#22c55e" : "#26263a";
    QString textColor = "#ffffff";
    QString nameColor = "#ffffff";
    QString timeColor = "#e5e7eb";
    QString title = sender;

    if (msg.getType() == MessageType::PrivateMessage) {
        title = isMe ? "Private to " + target : "Private from " + sender;
        bubbleColor = isMe ? "#7c6af7" : "#37306b";
    }

    if (isVoiceFileMessage(rawContent)) {
        content = renderVoiceContent(rawContent);
    }

    return QString(
        "<table width='100%' cellpadding='0' cellspacing='0'>"
        "<tr><td align='%1'>"
        "<table cellpadding='0' cellspacing='0' style='max-width:380px;'>"
        "<tr><td style='background:%2; color:%3; padding:12px 16px; "
        "border-radius:18px; font-size:14px; line-height:1.4;'>"
        "<div style='font-weight:bold; color:%4; margin-bottom:8px;'>%5</div>"
        "<div>%6</div>"
        "<div style='font-size:10px; color:%7; margin-top:8px; text-align:right;'>%8</div>"
        "</td></tr></table>"
        "</td></tr><tr><td height='6'></td></tr></table>"
    ).arg(align, bubbleColor, textColor, nameColor, title, content, timeColor, time);
}

QString MessageRenderer::renderGroupMessage(const QString& sender,
                                            const QString& content,
                                            const QString& time,
                                            bool isMe)
{
    QString safeSender = sender.toHtmlEscaped();
    QString rawContent = content;
    QString safeContent = content.toHtmlEscaped();
    QString safeTime = time.toHtmlEscaped();

    QString align = isMe ? "right" : "left";
    QString bubbleColor = isMe ? "#22c55e" : "#26263a";
    QString textColor = "#ffffff";
    QString nameColor = "#ffffff";
    QString timeColor = "#e5e7eb";

    if (isVoiceFileMessage(rawContent)) {
        safeContent = renderVoiceContent(rawContent);
    }

    return QString(
        "<table width='100%' cellpadding='0' cellspacing='0'>"
        "<tr><td align='%1'>"
        "<table cellpadding='0' cellspacing='0' style='max-width:380px;'>"
        "<tr><td style='background:%2; color:%3; padding:12px 16px; "
        "border-radius:18px; font-size:14px; line-height:1.4;'>"
        "<div style='font-weight:bold; color:%4; margin-bottom:8px;'>%5</div>"
        "<div>%6</div>"
        "<div style='font-size:10px; color:%7; margin-top:8px; text-align:right;'>%8</div>"
        "</td></tr></table>"
        "</td></tr><tr><td height='6'></td></tr></table>"
    ).arg(align, bubbleColor, textColor, nameColor, safeSender, safeContent, timeColor, safeTime);
}

QString MessageRenderer::renderTyping(const QString& text)
{
    return QString(
        "<div style='margin:8px 6px; color:#9aa0c3; font-size:12px; font-style:italic;'>"
        "💬 %1"
        "</div>"
    ).arg(text.toHtmlEscaped());
}
