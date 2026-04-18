#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_GroupWindow.h"

#include <QTimer>
#include <QStringList>

GroupWindow::GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QDialog(parent), ui(new Ui::Dialog), refreshTimer(new QTimer(this)), controller(controller)
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

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() {
                refreshGroups();
                refreshMessages();
            });

    refreshGroups();
    refreshMessages();
    refreshTimer->start(300);
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
    } else {
        ui->statusLabel->setText("Failed to create group");
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
    } else {
        ui->statusLabel->setText("Failed to join group");
    }
}

void GroupWindow::onSendClicked() {
    QString selectedGroup = ui->groupSelector->currentText().trimmed();
    QString content = ui->groupMessageInput->text().trimmed();

    if (selectedGroup.isEmpty()) {
        ui->statusLabel->setText("Select a group first");
        return;
    }

    if (content.isEmpty()) {
        ui->statusLabel->setText("Message cannot be empty");
        return;
    }

    if (controller->sendGroupMessage(selectedGroup.toStdString(), content.toStdString())) {
        ui->groupMessageInput->clear();
        ui->statusLabel->setText("Sent to group: " + selectedGroup);
        refreshMessages();
    } else {
        ui->statusLabel->setText("Failed to send group message");
    }
}

void GroupWindow::refreshGroups() {
    auto groups = controller->getState().getJoinedGroups();

    QString current = ui->groupSelector->currentText();
    QStringList newGroups;

    for (const auto& group : groups) {
        newGroups << QString::fromStdString(group);
    }

    QStringList existing;
    for (int i = 0; i < ui->groupSelector->count(); ++i) {
        existing << ui->groupSelector->itemText(i);
    }

    if (existing == newGroups) {
        return;
    }

    ui->groupSelector->clear();
    ui->groupSelector->addItems(newGroups);

    int index = ui->groupSelector->findText(current);
    if (index >= 0) {
        ui->groupSelector->setCurrentIndex(index);
    }
}

void GroupWindow::refreshMessages() {
    QString selectedGroup = ui->groupSelector->currentText().trimmed();

    if (selectedGroup.isEmpty()) {
        ui->groupDisplay->clear();
        return;
    }

    auto messages = controller->getState().getGroupMessagesForGroup(selectedGroup.toStdString());

    QStringList lines;
    for (const auto& msg : messages) {
        lines << QString::fromStdString(msg.getSender() + ": " + msg.getContent());
    }

    QString newText = lines.join("\n");

    if (ui->groupDisplay->toPlainText() == newText) {
        return;
    }

    ui->groupDisplay->setPlainText(newText);
}
