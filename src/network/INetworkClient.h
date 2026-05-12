#ifndef INETWORKCLIENT_H
#define INETWORKCLIENT_H

#include <string>
#include <functional>
#include <QByteArray>

class INetworkClient {
public:
    virtual ~INetworkClient() = default;

    virtual bool connectToServer(const std::string& username,
                                 const std::string& host,
                                 unsigned short port) = 0;

    virtual void sendMessage(const std::string& message) = 0;
    virtual void sendBinary(const QByteArray& data) = 0;

    virtual void setMessageHandler(std::function<void(const std::string&)> handler) = 0;
    virtual void setBinaryHandler(std::function<void(const QByteArray&)> handler) = 0;
};

#endif
