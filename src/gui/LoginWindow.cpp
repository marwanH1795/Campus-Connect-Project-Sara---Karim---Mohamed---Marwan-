#include "LoginWindow.h"
#include "ChatWindow.h"
#include "../logic/ChatController.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

LoginWindow::LoginWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent), controller(controller)
{
    setWindowTitle("Campus Connect - Login");
    resize(350, 200);

    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Enter username");

    connectButton = new QPushButton("Connect", this);

    statusLabel = new QLabel("Disconnected", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(usernameInput);
    layout->addWidget(connectButton);
    layout->addWidget(statusLabel);

    connect(connectButton, &QPushButton::clicked,
            this, [this]() { onConnectClicked(); });

    connect(usernameInput, &QLineEdit::returnPressed,
        this, [this]() { onConnectClicked(); });
}

void LoginWindow::onConnectClicked() {
    std::string username = usernameInput->text().toStdString();

    if (controller->connectUser(username)) {
        statusLabel->setText("Connected as: " + QString::fromStdString(username));

        ChatWindow* chatWindow = new ChatWindow(controller);
        chatWindow->show();

        this->close();
    } else {
        statusLabel->setText("Invalid username");
    }
}
