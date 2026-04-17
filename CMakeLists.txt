cmake_minimum_required(VERSION 3.16)

project(CampusConnect LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core)

include_directories(
    src
    src/gui
    src/logic
    src/network
)

set(PROJECT_SOURCES
    src/main.cpp

    src/gui/ChatWindow.cpp
    src/gui/ChatWindow.h

    src/gui/GroupWindow.cpp
    src/gui/GroupWindow.h

    src/gui/LoginWindow.cpp
    src/gui/LoginWindow.h
    src/gui/LoginWindow.ui

    src/logic/ChatController.cpp
    src/logic/ChatController.h

    src/logic/ChatState.cpp
    src/logic/ChatState.h

    src/logic/InputValidator.cpp
    src/logic/InputValidator.h

    src/logic/Message.cpp
    src/logic/Message.h

    src/network/INetworkClient.h

    src/network/MockNetworkClient.cpp
    src/network/MockNetworkClient.h

    src/network/TcpNetworkClient.cpp
    src/network/TcpNetworkClient.h

    resources.qrc
)

qt_add_executable(CampusConnect
    ${PROJECT_SOURCES}
)

target_link_libraries(CampusConnect PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::Core
)

add_executable(server
    server.cpp
)

target_link_libraries(server PRIVATE
    Qt6::Network
    Qt6::Core
)
