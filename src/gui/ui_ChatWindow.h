/********************************************************************************
** Form generated from reading UI file 'ChatWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *chatDisplay;
    QLineEdit *messageInput;
    QHBoxLayout *horizontalLayout;
    QPushButton *sendButton;
    QPushButton *groupButton;

    void setupUi(QWidget *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName("ChatWindow");
        ChatWindow->resize(552, 329);
        ChatWindow->setStyleSheet(QString::fromUtf8("\n"
"QWidget#ChatWindow {\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #e0e0f0;\n"
"    background-color: #1e1f2e;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200 Chat display \342\224\200\342\224\200 */\n"
"QTextEdit {\n"
"    background-color: #16172a;\n"
"    border: 2px solid #3a3b55;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #dcdcf0;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200 Message input \342\224\200\342\224\200 */\n"
"QLineEdit {\n"
"    background-color: #2a2b3d;\n"
"    color: #e0e0f0;\n"
"    border: 2px solid #4a4b6a;\n"
"    border-radius: 10px;\n"
"    padding: 4px 14px;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 2px solid #7c6af7;\n"
"}\n"
"QLineEdit[placeholderText] {\n"
"    color: #9a9ac0;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200 Send button \342\224\200\342\224\200 */\n"
"QPushButton#sendButton {"
                        "\n"
"    background-color: #3ecf8e;\n"
"    color: #1e1f2e;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"QPushButton#sendButton:hover {\n"
"    background-color: #55e0a3;\n"
"}\n"
"QPushButton#sendButton:pressed {\n"
"    background-color: #28a570;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200 Groups button \342\224\200\342\224\200 */\n"
"QPushButton#groupButton {\n"
"    background-color: #2a2b3d;\n"
"    color: #7c6af7;\n"
"    border: 2px solid #7c6af7;\n"
"    border-radius: 10px;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"QPushButton#groupButton:hover {\n"
"    background-color: #7c6af7;\n"
"    color: #ffffff;\n"
"}\n"
"QPushButton#groupButton:pressed {\n"
"    background-color: #5a4fcf;\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200 Scrollbars \342\224\200\342\224\200 */\n"
"QScrollBar:vertical {\n"
""
                        "    background: #1e1f2e;\n"
"    width: 8px;\n"
"    border-radius: 4px;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"    background: #4a4b6a;\n"
"    border-radius: 4px;\n"
"    min-height: 20px;\n"
"}\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #7c6af7;\n"
"}\n"
"QScrollBar::add-line:vertical,\n"
"QScrollBar::sub-line:vertical {\n"
"    height: 0px;\n"
"}\n"
"   "));
        verticalLayout = new QVBoxLayout(ChatWindow);
        verticalLayout->setSpacing(14);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        chatDisplay = new QTextEdit(ChatWindow);
        chatDisplay->setObjectName("chatDisplay");
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        chatDisplay->setFont(font);
        chatDisplay->setReadOnly(true);

        verticalLayout->addWidget(chatDisplay);

        messageInput = new QLineEdit(ChatWindow);
        messageInput->setObjectName("messageInput");
        messageInput->setMinimumSize(QSize(0, 46));

        verticalLayout->addWidget(messageInput);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(16);
        horizontalLayout->setObjectName("horizontalLayout");
        sendButton = new QPushButton(ChatWindow);
        sendButton->setObjectName("sendButton");
        sendButton->setMinimumSize(QSize(145, 48));
        sendButton->setMaximumSize(QSize(145, 48));

        horizontalLayout->addWidget(sendButton);

        groupButton = new QPushButton(ChatWindow);
        groupButton->setObjectName("groupButton");
        groupButton->setMinimumSize(QSize(145, 48));
        groupButton->setMaximumSize(QSize(145, 48));

        horizontalLayout->addWidget(groupButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QWidget *ChatWindow)
    {
        ChatWindow->setWindowTitle(QCoreApplication::translate("ChatWindow", "Campus Connect - Chat", nullptr));
        chatDisplay->setHtml(QCoreApplication::translate("ChatWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI','Arial','sans-serif'; font-size:13px; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Segoe UI'; font-size:12pt; font-style:italic; color:#9a9ac0;\">Messages will appear here\342\200\246</span><span style=\" font-family:'Segoe UI'; font-size:12pt; color:#9a9ac0;\"> </span></p></body></html>", nullptr));
        messageInput->setPlaceholderText(QCoreApplication::translate("ChatWindow", "Type a message...", nullptr));
        sendButton->setText(QCoreApplication::translate("ChatWindow", "Send  \342\236\244", nullptr));
        groupButton->setText(QCoreApplication::translate("ChatWindow", "Groups", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
