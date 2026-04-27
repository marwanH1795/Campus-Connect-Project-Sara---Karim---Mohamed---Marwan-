#include "ChatWindow.h"
#include "GroupWindow.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_ChatWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QTime>

ChatWindow::ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ChatWindow),
      refreshTimer(new QTimer(this)),
      controller(controller)
{
    ui->setupUi(this);

    connect(ui->sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(ui->groupButton, &QPushButton::clicked,
            this, [this]() { onOpenGroupWindow(); });

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(ui->messageInput, &QLineEdit::textChanged,
            this, [this]() { refreshMessages(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() { refreshMessages(); });

    refreshMessages();
    refreshTimer->start(200);
}

ChatWindow::~ChatWindow() {
    delete ui;
}

void ChatWindow::onSendClicked() {
    QString content = ui->messageInput->text().trimmed();

    if (content.isEmpty()) {
        return;
    }

    if (controller->sendPublicMessage(content.toStdString())) {
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

    QString currentUser = QString::fromStdString(controller->getCurrentUsername()).trimmed();
    QString currentUserLower = currentUser.toLower();

    QScrollBar* scrollBar = ui->chatDisplay->verticalScrollBar();
    bool shouldAutoScroll =
        scrollBar->value() >= scrollBar->maximum() - 25;

    QString html;
    html += "<html>";
    html += "<body style='background:#151525; color:white; font-family:Arial; margin:8px;'>";

    for (const auto& msg : messages) {
        QString sender = QString::fromStdString(msg.getSender()).toHtmlEscaped();
        QString senderLower = sender.toLower();
        QString content = QString::fromStdString(msg.getContent()).toHtmlEscaped();
        QString time = QTime::currentTime().toString("hh:mm");

        bool isMe = (senderLower == currentUserLower);

        QString outerAlign = isMe ? "right" : "left";
        QString bubbleColor = isMe ? "#3fcf8e" : "#2b2b3d";
        QString textColor = isMe ? "#06130f" : "#ffffff";
        QString nameColor = isMe ? "#06130f" : "#8b7cff";
        QString timeColor = isMe ? "#12352a" : "#b9b9c9";

        html += QString(
            "<table width='100%' cellpadding='4' cellspacing='0'>"
            "<tr>"
            "<td align='%1'>"

            "<table width='55%' cellpadding='0' cellspacing='0'>"
            "<tr>"
            "<td style='"
            "background-color:%2;"
            "color:%3;"
            "padding:12px;"
            "border-radius:18px;"
            "font-size:14px;"
            "line-height:1.35;"
            "'>"

            "<span style='font-weight:bold; color:%4;'>%5</span><br>"
            "<span>%6</span><br>"
            "<div style='font-size:10px; color:%7; margin-top:6px; text-align:right;'>%8</div>"

            "</td>"
            "</tr>"
            "</table>"

            "</td>"
            "</tr>"
            "</table>"
        )
        .arg(outerAlign)
        .arg(bubbleColor)
        .arg(textColor)
        .arg(nameColor)
        .arg(sender)
        .arg(content)
        .arg(timeColor)
        .arg(time);
    }

    if (!ui->messageInput->text().trimmed().isEmpty()) {
        html += QString(
            "<table width='100%' cellpadding='4' cellspacing='0'>"
            "<tr>"
            "<td align='right'>"
            "<span style='color:#9aa0c3; font-size:12px; font-style:italic;'>"
            "%1 is typing..."
            "</span>"
            "</td>"
            "</tr>"
            "</table>"
        ).arg(currentUser.toHtmlEscaped());
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) {
        return;
    }

    lastRenderedHtml = html;
    ui->chatDisplay->setHtml(html);

    if (shouldAutoScroll) {
        ui->chatDisplay->verticalScrollBar()->setValue(
            ui->chatDisplay->verticalScrollBar()->maximum()
        );
    }
}
