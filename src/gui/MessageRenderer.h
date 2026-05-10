#ifndef MESSAGERENDERER_H
#define MESSAGERENDERER_H

#include <QString>
#include "../logic/Message.h"

class MessageRenderer {
public:
    static QString renderMessage(const Message& msg, const QString& currentUser);

    static QString renderGroupMessage(const QString& sender,
                                      const QString& content,
                                      const QString& time,
                                      bool isMe);

    static QString renderTyping(const QString& text);
};

#endif
