/********************************************************************************
** Form generated from reading UI file 'LoginWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLineEdit *usernameInput;
    QPushButton *connectButton;
    QLabel *statusLabel;
    QFrame *frame;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(420, 300);
        LoginWindow->setMinimumSize(QSize(420, 280));
        LoginWindow->setMaximumSize(QSize(400, 300));
        LoginWindow->setStyleSheet(QString::fromUtf8("QWidget#LoginWindow {\n"
"    background-image: url(:/images/background.jpeg);\n"
"    background-repeat: no-repeat;\n"
"    background-position: center;\n"
"    color: white;\n"
"}"));
        layoutWidget = new QWidget(LoginWindow);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(110, 20, 193, 268));
        layoutWidget->setMinimumSize(QSize(151, 201));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(16);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(22, 24, 22, 24);
        titleLabel = new QLabel(layoutWidget);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setStyleSheet(QString::fromUtf8("color: white;"));
        titleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        subtitleLabel = new QLabel(layoutWidget);
        subtitleLabel->setObjectName("subtitleLabel");
        QFont font1;
        font1.setPointSize(13);
        subtitleLabel->setFont(font1);
        subtitleLabel->setStyleSheet(QString::fromUtf8("color: #d0d0d0;"));
        subtitleLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(subtitleLabel);

        usernameInput = new QLineEdit(layoutWidget);
        usernameInput->setObjectName("usernameInput");
        usernameInput->setMinimumSize(QSize(149, 25));
        usernameInput->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout->addWidget(usernameInput);

        connectButton = new QPushButton(layoutWidget);
        connectButton->setObjectName("connectButton");
        connectButton->setMinimumSize(QSize(149, 25));
        connectButton->setStyleSheet(QString::fromUtf8("background-color: #4f8cff;\n"
"color: white;\n"
"border-radius: 10px;\n"
"font-weight: bold;"));

        verticalLayout->addWidget(connectButton);

        statusLabel = new QLabel(layoutWidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setMinimumSize(QSize(149, 72));
        statusLabel->setMaximumSize(QSize(16777215, 16777215));
        statusLabel->setFont(font1);
        statusLabel->setStyleSheet(QString::fromUtf8("color: #d0d0d0;"));
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(statusLabel);

        frame = new QFrame(LoginWindow);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(110, 19, 191, 261));
        frame->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 0, 0, 0.6);\n"
"border-radius: 15px;"));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        frame->raise();
        layoutWidget->raise();

        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "Form", nullptr));
        titleLabel->setText(QCoreApplication::translate("LoginWindow", "Campus Connect", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("LoginWindow", "Login to continue", nullptr));
        usernameInput->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Enter username ", nullptr));
        connectButton->setText(QCoreApplication::translate("LoginWindow", "Connect", nullptr));
        statusLabel->setText(QCoreApplication::translate("LoginWindow", "Status:  Disconnected", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
