//We need to make this mock file in order to mock the call of connectToserver() function (we don't need a mock in the server side though)

#include "INetworkClient.h"
#include <gmock/gmock.h>

class MockNetworkClient : public INetworkClient {
public:
    MOCK_METHOD(void, connectToServer, (const std::string& username), (override));
    MOCK_METHOD(void, sendMessage, (const std::string& json), (override));
    MOCK_METHOD(void, setMessageHandler,
        (std::function<void(const std::string&)> handler), (override));
};
