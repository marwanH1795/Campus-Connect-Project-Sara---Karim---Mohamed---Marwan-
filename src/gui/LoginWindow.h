#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <memory>

class QLineEdit;
class QPushButton;
class QLabel;
class ChatController;

class LoginWindow : public QWidget {
private:
    QLineEdit* usernameInput;
    QPushButton* connectButton;
    QLabel* statusLabel;

    std::shared_ptr<ChatController> controller;

public:
    explicit LoginWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);

private:
    void onConnectClicked();
};

#endif
