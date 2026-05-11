#include <gtest/gtest.h>
#include "ChatState.h"
#include "Message.h"
TEST(ChatStateTest, UserCanJoinGroup) {
    ChatState state;
    state.joinGroup("room1");
    EXPECT_TRUE(state.isInGroup("room1"));
}
TEST(ChatStateTest, JoiningGroupTwiceNoDuplicate) {
    ChatState state;
    state.joinGroup("room1");
    state.joinGroup("room1");
    EXPECT_EQ(state.getJoinedGroups().size(), 1);
}
TEST(ChatStateTest, GroupMessageStoredForCorrectGroup) {
    ChatState state;
    state.joinGroup("room1");
    Message msg(MessageType::GroupMessage, "alice", "", "room1", "Hello group", "");
    state.addGroupMessage(msg);
    EXPECT_EQ(state.getGroupMessagesForGroup("room1").size(), 1);
    EXPECT_EQ(state.getGroupMessagesForGroup("room2").size(), 0);
}
TEST(ChatStateTest, PublicMessagesAccumulate) {
    ChatState state;
    state.addPublicMessage(Message(MessageType::PublicMessage, "alice", "", "", "Hi", ""));
    state.addPublicMessage(Message(MessageType::PublicMessage, "bob", "", "", "Hey", ""));
    EXPECT_EQ(state.getPublicMessages().size(), 2);
}
TEST(ChatStateTest, NotInGroupByDefault) {
    ChatState state;
    EXPECT_FALSE(state.isInGroup("unknown"));
}