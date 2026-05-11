#include "TcpNetworkClient.h"

#include <QDebug>
#include <QString>

TcpNetworkClient::TcpNetworkClient() {
    socket = new QTcpSocket();

    QObject::connect(socket, &QTcpSocket::readyRead, [this]() {
        buffer.append(socket->readAll());
        processBuffer();
    });

    QObject::connect(socket, &QTcpSocket::connected, [this]() {
        qDebug() << "Connected to server";

        for (const QString& msg : pendingMessages) {
            QByteArray data = msg.toUtf8();
            data.append('\n');
            socket->write(data);
        }

        pendingMessages.clear();
    });

    QObject::connect(socket, &QTcpSocket::disconnected, []() {
        qDebug() << "Disconnected from server";
    });
}

TcpNetworkClient::~TcpNetworkClient() {
    socket->close();
    delete socket;
}

void TcpNetworkClient::setMessageHandler(std::function<void(const std::string&)> handler) {
    messageHandler = handler;
}

void TcpNetworkClient::connectToServer(const std::string& username) {
    QString connectMessage =
        QString("{\"type\":\"connect\",\"sender\":\"%1\",\"target\":\"\",\"groupId\":\"\",\"content\":\"\",\"timestamp\":\"\"}")
            .arg(QString::fromStdString(username));

    pendingMessages.append(connectMessage);

    socket->connectToHost("127.0.0.1", 12345);
}

void TcpNetworkClient::sendMessage(const std::string& message) {
    QString qmsg = QString::fromStdString(message);

    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = qmsg.toUtf8();
        data.append('\n');
        socket->write(data);
    } else {
        pendingMessages.append(qmsg);
    }
}

void TcpNetworkClient::processBuffer() {
    while (true) {
        int index = buffer.indexOf('\n');

        if (index == -1) {
            break;
        }

        QByteArray line = buffer.left(index).trimmed();
        buffer.remove(0, index + 1);

        if (line.isEmpty()) {
            continue;
        }

        if (messageHandler) {
            messageHandler(line.toStdString());
        }
    }
}
