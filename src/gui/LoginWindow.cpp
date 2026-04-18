#include "LoginWindow.h"
#include "ChatWindow.h"
#include "../logic/ChatController.h"
#include "ui_LoginWindow.h"

LoginWindow::LoginWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent), ui(new Ui::LoginWindow), controller(controller)
{
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked,
            this, [this]() { onConnectClicked(); });

    connect(ui->usernameInput, &QLineEdit::returnPressed,
            this, [this]() { onConnectClicked(); });
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::onConnectClicked() {
    std::string username = ui->usernameInput->text().toStdString();

    if (controller->connectUser(username)) {
        ui->statusLabel->setText("Connected as: " + QString::fromStdString(username));

        ChatWindow* chatWindow = new ChatWindow(controller);
        chatWindow->show();

        this->close();
    } else {
        ui->statusLabel->setText("Invalid username");
    }
}
