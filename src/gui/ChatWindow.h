#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <memory>

class QTimer;
class ChatController;

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget {
private:
    Ui::ChatWindow* ui;
    QTimer* refreshTimer;
    std::shared_ptr<ChatController> controller;

public:
    explicit ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);
    ~ChatWindow();

private:
    void onSendClicked();
    void onOpenGroupWindow();
    void refreshMessages();
};

#endif
