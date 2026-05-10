#include "LoginWindow.h"
#include "ChatWindow.h"
#include "../logic/ChatController.h"
<<<<<<< HEAD

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
=======
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
>>>>>>> origin/Marwan

        ChatWindow* chatWindow = new ChatWindow(controller);
        chatWindow->show();

        this->close();
    } else {
<<<<<<< HEAD
        statusLabel->setText("Invalid username");
=======
        ui->statusLabel->setText("Invalid username");
>>>>>>> origin/Marwan
    }
}
