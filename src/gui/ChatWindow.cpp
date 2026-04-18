#include "ChatWindow.h"
#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_ChatWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QStringList>

ChatWindow::ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent), ui(new Ui::ChatWindow), refreshTimer(new QTimer(this)), controller(controller)
{
    ui->setupUi(this);

    connect(ui->sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(ui->groupButton, &QPushButton::clicked,
            this, [this]() { onOpenGroupWindow(); });

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() { refreshMessages(); });

    refreshMessages();
    refreshTimer->start(200);
}

ChatWindow::~ChatWindow() {
    delete ui;
}

void ChatWindow::onSendClicked() {
    QString qContent = ui->messageInput->text().trimmed();
    std::string content = qContent.toStdString();

    if (content.empty()) {
        return;
    }

    if (controller->sendPublicMessage(content)) {
        ui->messageInput->clear();
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

    if (ui->chatDisplay->toPlainText() == newText) {
        return;
    }

    ui->chatDisplay->setPlainText(newText);
    ui->chatDisplay->verticalScrollBar()->setValue(
        ui->chatDisplay->verticalScrollBar()->maximum()
    );
}
