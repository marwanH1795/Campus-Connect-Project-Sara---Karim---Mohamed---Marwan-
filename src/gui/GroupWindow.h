#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QWidget>
#include <memory>

class QLineEdit;
class QPushButton;
class QLabel;
class QTextEdit;
class QComboBox;
class QTimer;
class ChatController;

class GroupWindow : public QWidget {
private:
    QLineEdit* groupNameInput;
    QPushButton* createButton;
    QPushButton* joinButton;

    QComboBox* groupSelector;
    QTextEdit* groupDisplay;
    QLineEdit* groupMessageInput;
    QPushButton* sendButton;

    QLabel* statusLabel;
    QTimer* refreshTimer;

    std::shared_ptr<ChatController> controller;

public:
    explicit GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);

private:
    void onCreateClicked();
    void onJoinClicked();
    void onSendClicked();
    void refreshGroups();
    void refreshMessages();
};

#endif
