#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <memory>

<<<<<<< HEAD
class QLineEdit;
class QPushButton;
class QLabel;
=======
namespace Ui {
class LoginWindow;
}

>>>>>>> origin/Marwan
class ChatController;

class LoginWindow : public QWidget {
private:
<<<<<<< HEAD
    QLineEdit* usernameInput;
    QPushButton* connectButton;
    QLabel* statusLabel;

=======
    Ui::LoginWindow* ui;
>>>>>>> origin/Marwan
    std::shared_ptr<ChatController> controller;

public:
    explicit LoginWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);
<<<<<<< HEAD
=======
    ~LoginWindow();
>>>>>>> origin/Marwan

private:
    void onConnectClicked();
};

#endif
