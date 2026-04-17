#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <memory>

class QTextEdit;
class QLineEdit;
class QPushButton;
class QTimer;
class ChatController;

class ChatWindow : public QWidget {
private:
    QTextEdit* chatDisplay;
    QLineEdit* messageInput;
    QPushButton* sendButton;
    QPushButton* groupButton;
    QTimer* refreshTimer;

    std::shared_ptr<ChatController> controller;

public:
    explicit ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);

private:
    void onSendClicked();
    void onOpenGroupWindow();
    void refreshMessages();
};

#endif
