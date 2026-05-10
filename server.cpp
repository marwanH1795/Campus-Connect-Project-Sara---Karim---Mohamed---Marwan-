<<<<<<< HEAD
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QHash>
#include <QSet>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

QList<QTcpSocket*> clients;
QHash<QTcpSocket*, QByteArray> clientBuffers;
QHash<QString, QSet<QTcpSocket*>> groupMembers;

static void sendJson(QTcpSocket* client, const QJsonObject& obj) {
    if (!client || client->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    data.append('\n');
    client->write(data);
}

static void broadcastJson(const QJsonObject& obj) {
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    data.append('\n');

    for (QTcpSocket* c : clients) {
        if (c->state() == QAbstractSocket::ConnectedState) {
            c->write(data);
        }
    }
}

static void sendToGroup(const QString& groupId, const QJsonObject& obj) {
    if (!groupMembers.contains(groupId)) {
        return;
    }

    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    data.append('\n');

    for (QTcpSocket* c : groupMembers[groupId]) {
        if (c && c->state() == QAbstractSocket::ConnectedState) {
            c->write(data);
        }
    }
}

static void handleJsonMessage(QTcpSocket* client, const QJsonObject& obj) {
    QString type = obj.value("type").toString();
    QString groupId = obj.value("groupId").toString();

    qDebug() << "Received JSON:" << QJsonDocument(obj).toJson(QJsonDocument::Compact);

    if (type == "public_message") {
        broadcastJson(obj);
        return;
    }

    if (type == "create_group") {
        if (!groupId.isEmpty()) {
            groupMembers[groupId].insert(client);
            sendJson(client, obj);
            qDebug() << "Client added to created group:" << groupId;
        }
        return;
    }

    if (type == "join_group") {
        if (!groupId.isEmpty()) {
            groupMembers[groupId].insert(client);
            sendJson(client, obj);
            qDebug() << "Client joined group:" << groupId;
        }
        return;
    }

    if (type == "group_message") {
        if (!groupId.isEmpty() &&
            groupMembers.contains(groupId) &&
            groupMembers[groupId].contains(client)) {
            sendToGroup(groupId, obj);
            qDebug() << "Broadcasted group message to:" << groupId;
        } else {
            qDebug() << "Rejected group message for non-member group:" << groupId;
        }
        return;
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QTcpServer server;

    QObject::connect(&server, &QTcpServer::newConnection, [&]() {
        QTcpSocket* client = server.nextPendingConnection();
        clients.append(client);
        clientBuffers[client] = QByteArray();

        qDebug() << "New client connected";

        QObject::connect(client, &QTcpSocket::readyRead, [client]() {
            clientBuffers[client].append(client->readAll());

            while (true) {
                int newlineIndex = clientBuffers[client].indexOf('\n');
                if (newlineIndex == -1) {
                    break;
                }

                QByteArray line = clientBuffers[client].left(newlineIndex).trimmed();
                clientBuffers[client].remove(0, newlineIndex + 1);

                if (line.isEmpty()) {
                    continue;
                }

                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(line, &error);

                if (error.error != QJsonParseError::NoError || !doc.isObject()) {
                    qDebug() << "Invalid JSON received:" << line;
                    continue;
                }

                handleJsonMessage(client, doc.object());
            }
        });

        QObject::connect(client, &QTcpSocket::disconnected, [client]() {
            qDebug() << "Client disconnected";

            clients.removeAll(client);
            clientBuffers.remove(client);

            for (auto it = groupMembers.begin(); it != groupMembers.end(); ) {
                it.value().remove(client);
                if (it.value().isEmpty()) {
                    it = groupMembers.erase(it);
                } else {
                    ++it;
                }
            }

            client->deleteLater();
        });
    });

    if (!server.listen(QHostAddress::Any, 12345)) {
        qDebug() << "Server failed to start!";
        return 1;
    }

    qDebug() << "Server running on port 12345";

    return a.exec();
=======
#include <boost/asio/io_context.hpp>
#include <cstdio>
#include <exception>

#include "src/server/TcpServer.h"

int main() {
    try {
        boost::asio::io_context ioContext;

        TcpServer server(ioContext, 12345);

        ioContext.run();
    }
    catch (const std::exception& e) {
        std::printf("Server error: %s\n", e.what());
    }

    return 0;
>>>>>>> origin/Marwan
}
