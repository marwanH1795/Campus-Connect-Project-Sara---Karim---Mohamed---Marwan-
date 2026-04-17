#include "ChatWindow.h"
#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTimer>
#include <QStringList>

ChatWindow::ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent), controller(controller)
{
    setWindowTitle("Campus Connect - Chat");
    resize(500, 400);

    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);

    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Type a message");

    sendButton = new QPushButton("Send", this);
    groupButton = new QPushButton("Groups", this);

    refreshTimer = new QTimer(this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(sendButton);
    buttonLayout->addWidget(groupButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chatDisplay);
    layout->addWidget(messageInput);
    layout->addLayout(buttonLayout);

    connect(sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(groupButton, &QPushButton::clicked,
            this, [this]() { onOpenGroupWindow(); });

    connect(messageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() { refreshMessages(); });

    refreshMessages();
    refreshTimer->start(200);
}

void ChatWindow::onSendClicked() {
    QString qContent = messageInput->text().trimmed();
    std::string content = qContent.toStdString();

    if (content.empty()) {
        return;
    }

    if (controller->sendPublicMessage(content)) {
        messageInput->clear();
        refreshMessages();
    }
}

void ChatWindow::onOpenGroupWindow() {
    GroupWindow* groupWindow = new GroupWindow(controller);
    groupWindow->show();
}

void ChatWindow::refreshMessages() {
    auto messages = controller->getState().getPublicMessages();

    QStringList lines;
    for (const auto& msg : messages) {
        lines << QString::fromStdString(msg.getSender() + ": " + msg.getContent());
    }

    QString newText = lines.join("\n");

    if (chatDisplay->toPlainText() == newText) {
        return;
    }

    chatDisplay->setPlainText(newText);
    chatDisplay->verticalScrollBar()->setValue(
        chatDisplay->verticalScrollBar()->maximum()
    );
}
