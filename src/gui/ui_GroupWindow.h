/********************************************************************************
** Form generated from reading UI file 'GroupWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GROUPWINDOW_H
#define UI_GROUPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLineEdit *groupNameInput;
    QPushButton *createButton;
    QPushButton *joinButton;
    QComboBox *groupSelector;
    QTextEdit *groupDisplay;
    QLineEdit *groupMessageInput;
    QPushButton *sendButton;
    QLabel *statusLabel;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(705, 455);
        groupNameInput = new QLineEdit(Dialog);
        groupNameInput->setObjectName("groupNameInput");
        groupNameInput->setGeometry(QRect(30, 20, 300, 34));
        createButton = new QPushButton(Dialog);
        createButton->setObjectName("createButton");
        createButton->setGeometry(QRect(360, 20, 160, 34));
        joinButton = new QPushButton(Dialog);
        joinButton->setObjectName("joinButton");
        joinButton->setGeometry(QRect(30, 66, 160, 34));
        groupSelector = new QComboBox(Dialog);
        groupSelector->setObjectName("groupSelector");
        groupSelector->setGeometry(QRect(210, 66, 200, 34));
        groupDisplay = new QTextEdit(Dialog);
        groupDisplay->setObjectName("groupDisplay");
        groupDisplay->setGeometry(QRect(30, 110, 600, 200));
        groupDisplay->setReadOnly(true);
        groupMessageInput = new QLineEdit(Dialog);
        groupMessageInput->setObjectName("groupMessageInput");
        groupMessageInput->setGeometry(QRect(30, 330, 460, 36));
        sendButton = new QPushButton(Dialog);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(506, 330, 124, 36));
        statusLabel = new QLabel(Dialog);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(30, 400, 241, 31));
        QFont font;
        font.setBold(true);
        font.setItalic(true);
        statusLabel->setFont(font);
        statusLabel->setFrameShape(QFrame::Shape::StyledPanel);
        statusLabel->setLineWidth(0);
        statusLabel->setTextFormat(Qt::TextFormat::AutoText);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Group Chat", nullptr));
        Dialog->setStyleSheet(QCoreApplication::translate("Dialog", "\n"
"    QDialog {\n"
"      background-color: #1e1f2e;\n"
"      color: #e0e0f0;\n"
"      font-family: \"Segoe UI\", Arial, sans-serif;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Group name input \342\224\200\342\224\200 */\n"
"    QLineEdit#groupNameInput {\n"
"      background-color: #2a2b3d;\n"
"      color: #ffffff;\n"
"      border: 2px solid #4a4b6a;\n"
"      border-radius: 10px;\n"
"      padding: 4px 10px;\n"
"      font-size: 13px;\n"
"      selection-background-color: #7c6af7;\n"
"    }\n"
"    QLineEdit#groupNameInput:focus {\n"
"      border: 2px solid #7c6af7;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Message input \342\224\200\342\224\200 */\n"
"    QLineEdit#groupMessageInput {\n"
"      background-color: #2a2b3d;\n"
"      color: #e0e0f0;\n"
"      border: 2px solid #4a4b6a;\n"
"      border-radius: 10px;\n"
"      padding: 4px 12px;\n"
"      font-size: 12px;\n"
"    }\n"
"    QLineEdit#groupMessageInput:focus {\n"
"      border: 2px solid #7c6af7;\n"
"    }\n"
"\n"
"    /*"
                        " \342\224\200\342\224\200 Shared button base \342\224\200\342\224\200 */\n"
"    QPushButton {\n"
"      border-radius: 10px;\n"
"      padding: 5px 14px;\n"
"      font-size: 12px;\n"
"      font-weight: bold;\n"
"      font-family: \"Segoe UI\", Arial, sans-serif;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Create Group button \342\224\200\342\224\200 */\n"
"    QPushButton#createButton {\n"
"      background-color: #7c6af7;\n"
"      color: #ffffff;\n"
"      border: none;\n"
"    }\n"
"    QPushButton#createButton:hover {\n"
"      background-color: #9b8dff;\n"
"    }\n"
"    QPushButton#createButton:pressed {\n"
"      background-color: #5a4fcf;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Join Group button \342\224\200\342\224\200 */\n"
"    QPushButton#joinButton {\n"
"      background-color: #2a2b3d;\n"
"      color: #7c6af7;\n"
"      border: 2px solid #7c6af7;\n"
"    }\n"
"    QPushButton#joinButton:hover {\n"
"      background-color: #7c6af7;\n"
"      color: #ffffff;\n"
"    }\n"
"   "
                        " QPushButton#joinButton:pressed {\n"
"      background-color: #5a4fcf;\n"
"      color: #ffffff;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Send button \342\224\200\342\224\200 */\n"
"    QPushButton#sendButton {\n"
"      background-color: #3ecf8e;\n"
"      color: #1e1f2e;\n"
"      border: none;\n"
"    }\n"
"    QPushButton#sendButton:hover {\n"
"      background-color: #55e0a3;\n"
"    }\n"
"    QPushButton#sendButton:pressed {\n"
"      background-color: #28a570;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 ComboBox \342\224\200\342\224\200 */\n"
"    QComboBox#groupSelector {\n"
"      background-color: #2a2b3d;\n"
"      color: #e0e0f0;\n"
"      border: 2px solid #4a4b6a;\n"
"      border-radius: 10px;\n"
"      padding: 3px 10px;\n"
"      font-size: 12px;\n"
"    }\n"
"    QComboBox#groupSelector:focus {\n"
"      border: 2px solid #7c6af7;\n"
"    }\n"
"    QComboBox#groupSelector::drop-down {\n"
"      border: none;\n"
"      width: 24px;\n"
"    }\n"
"    QComboBox QAbstractItemView"
                        " {\n"
"      background-color: #2a2b3d;\n"
"      color: #e0e0f0;\n"
"      selection-background-color: #7c6af7;\n"
"      border: 1px solid #4a4b6a;\n"
"      border-radius: 6px;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Chat display \342\224\200\342\224\200 */\n"
"    QTextEdit#groupDisplay {\n"
"      background-color: #16172a;\n"
"      color: #dcdcf0;\n"
"      border: 2px solid #3a3b55;\n"
"      border-radius: 12px;\n"
"      padding: 8px;\n"
"      font-size: 12px;\n"
"      font-family: \"Segoe UI\", Arial, sans-serif;\n"
"    }\n"
"\n"
"    /* \342\224\200\342\224\200 Scrollbars \342\224\200\342\224\200 */\n"
"    QScrollBar:vertical {\n"
"      background: #1e1f2e;\n"
"      width: 8px;\n"
"      border-radius: 4px;\n"
"    }\n"
"    QScrollBar::handle:vertical {\n"
"      background: #4a4b6a;\n"
"      border-radius: 4px;\n"
"      min-height: 20px;\n"
"    }\n"
"    QScrollBar::handle:vertical:hover {\n"
"      background: #7c6af7;\n"
"    }\n"
"    QScrollBar::add-line:vertical, QScrollB"
                        "ar::sub-line:vertical {\n"
"      height: 0px;\n"
"    }\n"
"   ", nullptr));
        groupNameInput->setText(QCoreApplication::translate("Dialog", "My group", nullptr));
        groupNameInput->setPlaceholderText(QCoreApplication::translate("Dialog", "Group name...", nullptr));
        createButton->setText(QCoreApplication::translate("Dialog", "\357\274\213  Create Group", nullptr));
        joinButton->setText(QCoreApplication::translate("Dialog", "\342\206\222  Join Group", nullptr));
        groupDisplay->setHtml(QCoreApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI','Arial','sans-serif'; font-size:12px; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Segoe UI'; font-size:12pt; font-style:italic; color:#9a9ac0;\">Group messages will appear here\342\200\246</span><span style=\" font-family:'Segoe UI'; font-size:12pt; color:#9a9ac0;\"> </span></p></body></html>", nullptr));
        groupMessageInput->setPlaceholderText(QCoreApplication::translate("Dialog", "Type a message...", nullptr));
        sendButton->setText(QCoreApplication::translate("Dialog", "Send  \342\236\244", nullptr));
        statusLabel->setText(QCoreApplication::translate("Dialog", "Sent to group: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GROUPWINDOW_H
