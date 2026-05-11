#ifndef INETWORKCLIENT_H
#define INETWORKCLIENT_H

#include <string>
#include <functional>

class INetworkClient {
public:
    virtual ~INetworkClient() = default;

    // connect user
    virtual void connectToServer(const std::string& username) = 0;

    // send message (JSON string)
    virtual void sendMessage(const std::string& message) = 0;

    // callback for receiving messages
    virtual void setMessageHandler(std::function<void(const std::string&)> handler) = 0;
};

#endif
