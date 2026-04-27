#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_GroupWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QTime>

GroupWindow::GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::Dialog),
      refreshTimer(new QTimer(this)),
      controller(controller)
{
    ui->setupUi(this);

    connect(ui->createButton, &QPushButton::clicked,
            this, [this]() { onCreateClicked(); });

    connect(ui->joinButton, &QPushButton::clicked,
            this, [this]() { onJoinClicked(); });

    connect(ui->sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(ui->groupNameInput, &QLineEdit::returnPressed,
            this, [this]() { onCreateClicked(); });

    connect(ui->groupMessageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(ui->groupSelector, &QComboBox::currentTextChanged,
            this, [this]() { refreshMessages(); });

    connect(ui->groupMessageInput, &QLineEdit::textChanged,
            this, [this]() { refreshMessages(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() {
                refreshGroups();
                refreshMessages();
            });

    refreshGroups();
    refreshMessages();
    refreshTimer->start(250);
}

GroupWindow::~GroupWindow() {
    delete ui;
}

void GroupWindow::onCreateClicked() {
    QString groupName = ui->groupNameInput->text().trimmed();

    if (groupName.isEmpty()) {
        ui->statusLabel->setText("Group name cannot be empty");
        return;
    }

    if (controller->createGroup(groupName.toStdString())) {
        ui->statusLabel->setText("Created group: " + groupName);
        ui->groupNameInput->clear();
    }
}

void GroupWindow::onJoinClicked() {
    QString groupName = ui->groupNameInput->text().trimmed();

    if (groupName.isEmpty()) {
        ui->statusLabel->setText("Group name cannot be empty");
        return;
    }

    if (controller->joinGroup(groupName.toStdString())) {
        ui->statusLabel->setText("Joined group: " + groupName);
        ui->groupNameInput->clear();
    }
}

void GroupWindow::onSendClicked() {
    QString group = ui->groupSelector->currentText().trimmed();
    QString content = ui->groupMessageInput->text().trimmed();

    if (group.isEmpty()) {
        ui->statusLabel->setText("Select a group first");
        return;
    }

    if (content.isEmpty()) {
        ui->statusLabel->setText("Message cannot be empty");
        return;
    }

    if (controller->sendGroupMessage(group.toStdString(), content.toStdString())) {
        ui->groupMessageInput->clear();
        refreshMessages();
    }
}

void GroupWindow::refreshGroups() {
    auto groups = controller->getState().getJoinedGroups();

    QString current = ui->groupSelector->currentText();
    ui->groupSelector->clear();

    for (const auto& g : groups) {
        ui->groupSelector->addItem(QString::fromStdString(g));
    }

    int index = ui->groupSelector->findText(current);
    if (index >= 0) {
        ui->groupSelector->setCurrentIndex(index);
    }
}

void GroupWindow::refreshMessages() {
    QString group = ui->groupSelector->currentText().trimmed();

    if (group.isEmpty()) {
        ui->groupDisplay->clear();
        return;
    }

    auto messages = controller->getState().getGroupMessagesForGroup(group.toStdString());

    QString currentUser = QString::fromStdString(controller->getCurrentUsername()).toLower();

    QScrollBar* scrollBar = ui->groupDisplay->verticalScrollBar();
    bool shouldAutoScroll = scrollBar->value() >= scrollBar->maximum() - 25;

    QString html;
    html += "<html><body style='background:#151525; color:white; font-family:Arial;'>";

    for (const auto& msg : messages) {
        QString sender = QString::fromStdString(msg.getSender()).toHtmlEscaped();
        QString senderLower = sender.toLower();
        QString content = QString::fromStdString(msg.getContent()).toHtmlEscaped();
        QString time = QTime::currentTime().toString("hh:mm");

        bool isMe = (senderLower == currentUser);

        QString align = isMe ? "right" : "left";
        QString bubble = isMe ? "#3fcf8e" : "#2b2b3d";

        html += QString(
            "<table width='100%' cellpadding='6'>"
            "<tr><td align='%1'>"
            "<table width='55%' cellpadding='10' style='background:%2; border-radius:16px;'>"
            "<tr><td>"
            "<b>%3</b><br>%4<br>"
            "<span style='font-size:10px; color:#aaa;'>%5</span>"
            "</td></tr></table>"
            "</td></tr></table>"
        )
        .arg(align)
        .arg(bubble)
        .arg(sender)
        .arg(content)
        .arg(time);
    }

    // typing indicator
    if (!ui->groupMessageInput->text().trimmed().isEmpty()) {
        html += QString(
            "<div style='text-align:right; color:#888; font-size:12px;'>%1 is typing...</div>"
        ).arg(QString::fromStdString(controller->getCurrentUsername()));
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) return;

    lastRenderedHtml = html;
    ui->groupDisplay->setHtml(html);

    if (shouldAutoScroll) {
        ui->groupDisplay->verticalScrollBar()->setValue(
            ui->groupDisplay->verticalScrollBar()->maximum()
        );
    }
}
