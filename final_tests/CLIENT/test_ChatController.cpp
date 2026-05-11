#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ChatController.h"
#include "MockNetworkClient.h"

using ::testing::_;
using ::testing::SaveArg;
TEST(ChatControllerTest, RejectsEmptyUsername) {
    auto mock = std::make_shared<MockNetworkClient>();
    EXPECT_CALL(*mock, setMessageHandler(_));
    EXPECT_CALL(*mock, connectToServer(_)).Times(0);

    ChatController ctrl(mock);
    EXPECT_FALSE(ctrl.connectUser(""));
}
TEST(ChatControllerTest, ValidUsernameCallsConnect) {
    auto mock = std::make_shared<MockNetworkClient>();
    EXPECT_CALL(*mock, setMessageHandler(_));
    EXPECT_CALL(*mock, connectToServer("alice")).Times(1);

    ChatController ctrl(mock);
    EXPECT_TRUE(ctrl.connectUser("alice"));
}
TEST(ChatControllerTest, SendPublicMessageCallsSendMessage) {
    auto mock = std::make_shared<MockNetworkClient>();
    EXPECT_CALL(*mock, setMessageHandler(_));
    EXPECT_CALL(*mock, connectToServer(_));
    EXPECT_CALL(*mock, sendMessage(_)).Times(1);

    ChatController ctrl(mock);
    ctrl.connectUser("alice");
    EXPECT_TRUE(ctrl.sendPublicMessage("Hello world"));
}
TEST(ChatControllerTest, RejectsEmptyPublicMessage) {
    auto mock = std::make_shared<MockNetworkClient>();
    EXPECT_CALL(*mock, setMessageHandler(_));
    EXPECT_CALL(*mock, sendMessage(_)).Times(0);

    ChatController ctrl(mock);
    EXPECT_FALSE(ctrl.sendPublicMessage(""));
}
TEST(ChatControllerTest, GroupMessageFailsIfNotInGroup) {
    auto mock = std::make_shared<MockNetworkClient>();
    EXPECT_CALL(*mock, setMessageHandler(_));
    EXPECT_CALL(*mock, sendMessage(_)).Times(0);

    ChatController ctrl(mock);
    EXPECT_FALSE(ctrl.sendGroupMessage("room1", "Hey!"));
}
