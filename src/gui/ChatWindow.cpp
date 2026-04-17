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
    resize(760, 560);

    setStyleSheet(
        "QWidget {"
        "background-image:url(:/src/assets/chatbg.jpeg);"
        "background-position:center;"
        "background-repeat:no-repeat;"
        "font-family:'Segoe UI';"
        "color:white;"
        "}"

        "QTextEdit {"
        "background:rgba(0,0,0,0.35);"
        "border:1px solid rgba(255,255,255,0.12);"
        "border-radius:18px;"
        "padding:14px;"
        "font-size:15px;"
        "}"

        "QLineEdit {"
        "background:rgba(0,0,0,0.30);"
        "border:none;"
        "border-bottom:2px solid #3ea6ff;"
        "padding:10px;"
        "font-size:15px;"
        "color:white;"
        "}"

        "QLineEdit::placeholder {"
        "color:#dddddd;"
        "}"
        );

    chatDisplay = new QTextEdit(this);
    chatDisplay->setFont(QFont("Segoe UI", 14));
    chatDisplay->setReadOnly(true);

    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Type a message...");
    messageInput->setMinimumHeight(48);

    sendButton = new QPushButton("Send", this);
    sendButton->setFixedSize(145, 52);
    sendButton->setStyleSheet(
        "QPushButton {"
        "background:#2492ff;"
        "color:white;"
        "border:none;"
        "border-radius:14px;"
        "font-size:18px;"
        "font-weight:700;"
        "}"
        "QPushButton:hover {"
        "background:#167de8;"
        "}"
        );

    groupButton = new QPushButton("Groups", this);
    groupButton->setFixedSize(145, 52);
    groupButton->setStyleSheet(
        "QPushButton {"
        "background:rgba(0,0,0,0.35);"
        "color:white;"
        "border:1px solid rgba(255,255,255,0.12);"
        "border-radius:14px;"
        "font-size:18px;"
        "font-weight:700;"
        "}"
        "QPushButton:hover {"
        "background:rgba(255,255,255,0.10);"
        "}"
        );

    refreshTimer = new QTimer(this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(16);
    buttonLayout->addStretch();
    buttonLayout->addWidget(sendButton);
    buttonLayout->addWidget(groupButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20,20,20,20);
    layout->setSpacing(14);

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

void ChatWindow::refreshMessages()
{
    auto messages = controller->getState().getPublicMessages();

    QString html = "";

    for (const auto& msg : messages)
    {
        QString sender = QString::fromStdString(msg.getSender());
        QString content = QString::fromStdString(msg.getContent());

        html += "<p style='margin:6px 0;'>"
                "<span style='color:#B88CFF; font-weight:bold;'>"
                + sender +
                ":</span> "
                "<span style='color:#DDEBFF;'>"
                + content +
                "</span></p>";
    }

    QScrollBar* bar = chatDisplay->verticalScrollBar();

    bool wasAtBottom =
        bar->value() >= bar->maximum() - 20;

    chatDisplay->clear();
    chatDisplay->insertHtml(html);

    if (wasAtBottom)
    {
        bar->setValue(bar->maximum());
    }
}

