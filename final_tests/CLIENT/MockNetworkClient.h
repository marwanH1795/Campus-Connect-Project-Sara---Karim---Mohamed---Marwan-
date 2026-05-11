#ifndef MOCKNETWORKCLIENT_H
#define MOCKNETWORKCLIENT_H

#include "INetworkClient.h"

#include <gmock/gmock.h>
#include <QByteArray>
#include <functional>

class MockNetworkClient : public INetworkClient {
public:
    MOCK_METHOD(void, connectToServer,
                (const std::string& username, const std::string& host, unsigned short port),
                (override));

    MOCK_METHOD(void, sendMessage,
                (const std::string& json),
                (override));

    MOCK_METHOD(void, setMessageHandler,
                (std::function<void(const std::string&)> handler),
                (override));


    MOCK_METHOD(void, sendBinary,
                (const QByteArray& data),
                (override));

    MOCK_METHOD(void, setBinaryHandler,
                (std::function<void(const QByteArray&)> handler),
                (override));
};

#endif
