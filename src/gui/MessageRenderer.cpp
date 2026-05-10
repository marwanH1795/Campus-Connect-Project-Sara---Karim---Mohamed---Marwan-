#include "MessageRenderer.h"

#include <QTime>
#include <QByteArray>
#include <QUrl>
#include <QFileInfo>

static QString protectPercent(QString text) {
    return text.replace("%", "%%");
}

static bool startsWithAttachmentPrefix(const QString& content) {
    return content.startsWith("VOICE_FILE|") ||
           content.startsWith("IMAGE_FILE|") ||
           content.startsWith("VIDEO_FILE|") ||
           content.startsWith("FILE_ATTACHMENT|");
}

static QString attachmentPrefix(const QString& content) {
    int firstSeparator = content.indexOf('|');

    if (firstSeparator <= 0) {
        return "";
    }

    return content.left(firstSeparator);
}

static QString attachmentFileName(const QString& content) {
    int firstSeparator = content.indexOf('|');

    if (firstSeparator < 0) {
        return "attachment";
    }

    int secondSeparator = content.indexOf('|', firstSeparator + 1);

    if (secondSeparator < 0) {
        return "attachment";
    }

    return content.mid(firstSeparator + 1, secondSeparator - firstSeparator - 1).toHtmlEscaped();
}

static QString attachmentFilePath(const QString& content) {
    int firstSeparator = content.indexOf('|');

    if (firstSeparator < 0) {
        return "";
    }

    int secondSeparator = content.indexOf('|', firstSeparator + 1);

    if (secondSeparator < 0) {
        return "";
    }

    return content.mid(secondSeparator + 1);
}

static QString encodedPathForLink(const QString& filePath) {
    return QString::fromLatin1(filePath.toUtf8().toBase64());
}

static QString renderVoiceContent(const QString& content) {
    QString name = attachmentFileName(content);
    QString path = attachmentFilePath(content);
    QString encodedPath = encodedPathForLink(path);

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

static QString renderImageContent(const QString& content) {
    QString name = attachmentFileName(content);
    QString path = attachmentFilePath(content);
    QString encodedPath = encodedPathForLink(path);
    QString fileUrl = QUrl::fromLocalFile(path).toString();

    return QString(
        "<a href='open:%1' style='text-decoration:none;'>"
        "<img src='%2' style='max-width:280px; max-height:220px; "
        "border-radius:14px; border:1px solid rgba(255,255,255,0.25);'/>"
        "</a>"
        "<div style='font-size:10px; margin-top:8px; opacity:0.85;'>🖼 %3</div>"
    ).arg(encodedPath, fileUrl, name);
}

static QString renderVideoContent(const QString& content) {
    QString name = attachmentFileName(content);
    QString path = attachmentFilePath(content);
    QString encodedPath = encodedPathForLink(path);

    return QString(
        "<a href='open:%1' style='text-decoration:none;'>"
        "<span style='background-color:#ffffff; color:#111827; "
        "padding:7px 12px; border-radius:14px; font-weight:bold;'>"
        "🎬 Open video"
        "</span>"
        "</a>"
        "<div style='font-size:10px; margin-top:8px; opacity:0.85;'>%2</div>"
    ).arg(encodedPath, name);
}

static QString renderFileContent(const QString& content) {
    QString name = attachmentFileName(content);
    QString path = attachmentFilePath(content);
    QString encodedPath = encodedPathForLink(path);

    return QString(
        "<a href='open:%1' style='text-decoration:none;'>"
        "<span style='background-color:#ffffff; color:#111827; "
        "padding:7px 12px; border-radius:14px; font-weight:bold;'>"
        "📎 Open file"
        "</span>"
        "</a>"
        "<div style='font-size:10px; margin-top:8px; opacity:0.85;'>%2</div>"
    ).arg(encodedPath, name);
}

static QString renderAttachmentContent(const QString& content) {
    QString prefix = attachmentPrefix(content);

    if (prefix == "VOICE_FILE") {
        return renderVoiceContent(content);
    }

    if (prefix == "IMAGE_FILE") {
        return renderImageContent(content);
    }

    if (prefix == "VIDEO_FILE") {
        return renderVideoContent(content);
    }

    return renderFileContent(content);
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

    if (startsWithAttachmentPrefix(rawContent)) {
        content = renderAttachmentContent(rawContent);
    } else {
        content = protectPercent(content);
    }

    return QString(
        "<table width='100%' cellpadding='0' cellspacing='0'>"
        "<tr><td align='%1'>"
        "<table cellpadding='0' cellspacing='0' style='max-width:420px;'>"
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

    if (startsWithAttachmentPrefix(rawContent)) {
        safeContent = renderAttachmentContent(rawContent);
    } else {
        safeContent = protectPercent(safeContent);
    }

    return QString(
        "<table width='100%' cellpadding='0' cellspacing='0'>"
        "<tr><td align='%1'>"
        "<table cellpadding='0' cellspacing='0' style='max-width:420px;'>"
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
