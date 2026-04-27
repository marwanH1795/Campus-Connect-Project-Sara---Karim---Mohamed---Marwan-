#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QDialog>
#include <QString>
#include <memory>

class QTimer;
class ChatController;

namespace Ui {
class Dialog;
}

class GroupWindow : public QDialog {
private:
    Ui::Dialog* ui;
    QTimer* refreshTimer;
    std::shared_ptr<ChatController> controller;

    QString lastRenderedHtml;
    QString lastSelectedGroup;

public:
    explicit GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);
    ~GroupWindow();

private:
    void onCreateClicked();
    void onJoinClicked();
    void onSendClicked();
    void refreshGroups();
    void refreshMessages();
};

#endif
