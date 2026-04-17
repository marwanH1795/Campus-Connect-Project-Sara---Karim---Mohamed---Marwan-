#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QComboBox>
#include <QTimer>
#include <QStringList>

GroupWindow::GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent), controller(controller)
{
    setWindowTitle("Campus Connect - Group");
    resize(500, 450);

    groupNameInput = new QLineEdit(this);
    groupNameInput->setPlaceholderText("Enter group name");

    createButton = new QPushButton("Create Group", this);
    joinButton = new QPushButton("Join Group", this);

    groupSelector = new QComboBox(this);

    groupDisplay = new QTextEdit(this);
    groupDisplay->setReadOnly(true);

    groupMessageInput = new QLineEdit(this);
    groupMessageInput->setPlaceholderText("Type a group message");

    sendButton = new QPushButton("Send to Group", this);

    statusLabel = new QLabel("No group selected", this);
    refreshTimer = new QTimer(this);

    QHBoxLayout* topButtons = new QHBoxLayout();
    topButtons->addWidget(createButton);
    topButtons->addWidget(joinButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(groupNameInput);
    layout->addLayout(topButtons);
    layout->addWidget(new QLabel("Joined groups:", this));
    layout->addWidget(groupSelector);
    layout->addWidget(groupDisplay);
    layout->addWidget(groupMessageInput);
    layout->addWidget(sendButton);
    layout->addWidget(statusLabel);

    connect(createButton, &QPushButton::clicked,
            this, [this]() { onCreateClicked(); });

    connect(joinButton, &QPushButton::clicked,
            this, [this]() { onJoinClicked(); });

    connect(sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(groupNameInput, &QLineEdit::returnPressed,
            this, [this]() { onCreateClicked(); });

    connect(groupMessageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(groupSelector, &QComboBox::currentTextChanged,
            this, [this]() { refreshMessages(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() {
                refreshGroups();
                refreshMessages();
            });

    refreshGroups();
    refreshMessages();
    refreshTimer->start(300);
}

void GroupWindow::onCreateClicked() {
    QString groupName = groupNameInput->text().trimmed();

    if (groupName.isEmpty()) {
        statusLabel->setText("Group name cannot be empty");
        return;
    }

    if (controller->createGroup(groupName.toStdString())) {
        statusLabel->setText("Create request sent: " + groupName);
        groupNameInput->clear();
    } else {
        statusLabel->setText("Failed to create group");
    }
}

void GroupWindow::onJoinClicked() {
    QString groupName = groupNameInput->text().trimmed();

    if (groupName.isEmpty()) {
        statusLabel->setText("Group name cannot be empty");
        return;
    }

    if (controller->joinGroup(groupName.toStdString())) {
        statusLabel->setText("Join request sent: " + groupName);
        groupNameInput->clear();
    } else {
        statusLabel->setText("Failed to join group");
    }
}

void GroupWindow::onSendClicked() {
    QString selectedGroup = groupSelector->currentText().trimmed();
    QString content = groupMessageInput->text().trimmed();

    if (selectedGroup.isEmpty()) {
        statusLabel->setText("Select a group first");
        return;
    }

    if (content.isEmpty()) {
        statusLabel->setText("Message cannot be empty");
        return;
    }

    if (controller->sendGroupMessage(selectedGroup.toStdString(), content.toStdString())) {
        groupMessageInput->clear();
        statusLabel->setText("Sent to group: " + selectedGroup);
        refreshMessages();
    } else {
        statusLabel->setText("Failed to send group message");
    }
}

void GroupWindow::refreshGroups() {
    auto groups = controller->getState().getJoinedGroups();

    QString current = groupSelector->currentText();
    QStringList newGroups;

    for (const auto& group : groups) {
        newGroups << QString::fromStdString(group);
    }

    QStringList existing;
    for (int i = 0; i < groupSelector->count(); ++i) {
        existing << groupSelector->itemText(i);
    }

    if (existing == newGroups) {
        return;
    }

    groupSelector->clear();
    groupSelector->addItems(newGroups);

    int index = groupSelector->findText(current);
    if (index >= 0) {
        groupSelector->setCurrentIndex(index);
    }
}

void GroupWindow::refreshMessages() {
    QString selectedGroup = groupSelector->currentText().trimmed();

    if (selectedGroup.isEmpty()) {
        groupDisplay->clear();
        return;
    }

    auto messages = controller->getState().getGroupMessagesForGroup(selectedGroup.toStdString());

    QStringList lines;
    for (const auto& msg : messages) {
        lines << QString::fromStdString(msg.getSender() + ": " + msg.getContent());
    }

    QString newText = lines.join("\n");

    if (groupDisplay->toPlainText() == newText) {
        return;
    }

    groupDisplay->setPlainText(newText);
}
