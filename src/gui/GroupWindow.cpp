#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_GroupWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QTime>
#include <QStringList>

static QString makeGroupBubbleHtml(const QString& sender,
                                   const QString& content,
                                   const QString& time,
                                   bool isMe)
{
    QString align = isMe ? "right" : "left";
    QString bubbleColor = isMe ? "#3fcf8e" : "#2b2b3d";
    QString textColor = isMe ? "#06130f" : "#ffffff";
    QString nameColor = isMe ? "#06130f" : "#8b7cff";
    QString timeColor = isMe ? "#164030" : "#b9b9c9";

    return QString(
        "<table width='100%' cellpadding='2' cellspacing='0'>"
        "<tr>"
        "<td align='%1'>"
        "<table cellpadding='0' cellspacing='0' style='max-width:420px;'>"
        "<tr>"
        "<td style='"
        "background-color:%2;"
        "color:%3;"
        "padding:9px 13px;"
        "border-radius:18px;"
        "font-size:14px;"
        "line-height:1.35;"
        "'>"
        "<span style='font-weight:bold; color:%4;'>%5</span><br>"
        "<span>%6</span>"
        "<div style='font-size:10px; color:%7; margin-top:4px; text-align:right;'>%8</div>"
        "</td>"
        "</tr>"
        "</table>"
        "</td>"
        "</tr>"
        "<tr><td height='4'></td></tr>"
        "</table>"
    )
    .arg(align)
    .arg(bubbleColor)
    .arg(textColor)
    .arg(nameColor)
    .arg(sender)
    .arg(content)
    .arg(timeColor)
    .arg(time);
}

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

    connect(ui->groupMessageInput, &QLineEdit::textChanged,
            this, [this]() { refreshMessages(); });

    connect(ui->groupSelector, &QComboBox::currentTextChanged,
            this, [this]() {
                lastRenderedHtml.clear();
                refreshMessages();
            });

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

    QStringList existingGroups;
    for (int i = 0; i < ui->groupSelector->count(); ++i) {
        existingGroups << ui->groupSelector->itemText(i);
    }

    if (existingGroups == newGroups) {
        return;
    }

    ui->groupSelector->clear();
    ui->groupSelector->addItems(newGroups);

    int index = ui->groupSelector->findText(current);
    if (index >= 0) {
        ui->groupSelector->setCurrentIndex(index);
    } else if (!newGroups.isEmpty()) {
        ui->groupSelector->setCurrentIndex(0);
    }

    lastRenderedHtml.clear();
}

void GroupWindow::refreshMessages() {
    QString selectedGroup = ui->groupSelector->currentText().trimmed();

    if (selectedGroup.isEmpty()) {
        ui->groupDisplay->clear();
        lastRenderedHtml.clear();
        return;
    }

    auto messages = controller->getState().getGroupMessagesForGroup(selectedGroup.toStdString());

    QString currentUser = QString::fromStdString(controller->getCurrentUsername()).trimmed().toLower();

    QScrollBar* scrollBar = ui->groupDisplay->verticalScrollBar();
    bool shouldAutoScroll = scrollBar->value() >= scrollBar->maximum() - 25;

    QString html;
    html += "<html><body style='background:#151525; color:white; font-family:Arial; margin:12px;'>";

    for (const auto& msg : messages) {
        QString sender = QString::fromStdString(msg.getSender()).toHtmlEscaped();
        QString content = QString::fromStdString(msg.getContent()).toHtmlEscaped();
        QString time = QString::fromStdString(msg.getTimestamp()).toHtmlEscaped();

        if (time.isEmpty()) {
            time = QTime::currentTime().toString("hh:mm");
        }

        bool isMe = (sender.toLower() == currentUser);

        html += makeGroupBubbleHtml(sender, content, time, isMe);
    }

    if (!ui->groupMessageInput->text().trimmed().isEmpty()) {
        html += QString(
            "<table width='100%' cellpadding='2' cellspacing='0'>"
            "<tr><td align='right'>"
            "<span style='color:#9aa0c3; font-size:12px; font-style:italic;'>"
            "%1 is typing in %2..."
            "</span>"
            "</td></tr>"
            "</table>"
        )
        .arg(QString::fromStdString(controller->getCurrentUsername()).toHtmlEscaped())
        .arg(selectedGroup.toHtmlEscaped());
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) {
        return;
    }

    lastRenderedHtml = html;
    ui->groupDisplay->setHtml(html);

    if (shouldAutoScroll) {
        ui->groupDisplay->verticalScrollBar()->setValue(
            ui->groupDisplay->verticalScrollBar()->maximum()
        );
    }
}
