#ifndef INETWORKCLIENT_H
#define INETWORKCLIENT_H

#include <string>
#include <functional>
#include <QByteArray>

class INetworkClient {
public:
    virtual ~INetworkClient() = default;

    // connect user
    virtual void connectToServer(const std::string& username,
                                 const std::string& host,
                                 unsigned short port) = 0;

    // send message (JSON string)
    virtual void sendMessage(const std::string& message) = 0;
    virtual void sendBinary(const QByteArray& data) = 0;

    // callback for receiving messages
    virtual void setMessageHandler(std::function<void(const std::string&)> handler) = 0;
    virtual void setBinaryHandler(std::function<void(const QByteArray&)> handler) = 0;
};

#endif
