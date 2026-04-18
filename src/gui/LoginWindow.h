#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <memory>

namespace Ui {
class LoginWindow;
}

class ChatController;

class LoginWindow : public QWidget {
private:
    Ui::LoginWindow* ui;
    std::shared_ptr<ChatController> controller;

public:
    explicit LoginWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);
    ~LoginWindow();

private:
    void onConnectClicked();
};

#endif
