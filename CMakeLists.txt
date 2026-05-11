cmake_minimum_required(VERSION 3.16)

project(CampusConnect LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Core
    Network
    Gui
    Multimedia
    MultimediaWidgets
)

add_executable(app
    src/main.cpp

    src/gui/LoginWindow.cpp
    src/gui/LoginWindow.h
    src/gui/LoginWindow.ui

    src/gui/ChatWindow.cpp
    src/gui/ChatWindow.h
    src/gui/ChatWindow.ui

    src/gui/GroupWindow.cpp
    src/gui/GroupWindow.h
    src/gui/GroupWindow.ui

    src/gui/PrivateWindow.cpp
    src/gui/PrivateWindow.h
    src/gui/PrivateWindow.ui

    src/gui/MessageRenderer.cpp
    src/gui/MessageRenderer.h

    src/gui/resources.qrc

    src/logic/ChatController.cpp
    src/logic/ChatController.h
    src/logic/ChatState.cpp
    src/logic/ChatState.h
    src/logic/InputValidator.cpp
    src/logic/InputValidator.h
    src/logic/Message.cpp
    src/logic/Message.h

    src/network/INetworkClient.h
    src/network/TcpNetworkClient.cpp
    src/network/TcpNetworkClient.h
    src/network/MockNetworkClient.cpp
    src/network/MockNetworkClient.h
)

target_include_directories(app PRIVATE
    src/gui
    src/logic
    src/network
)

target_link_libraries(app PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Network
    Qt6::Gui
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

add_executable(server
    server.cpp

    src/server/TcpServer.cpp
    src/server/TcpServer.h

    src/server/ClientSession.cpp
    src/server/ClientSession.h

    src/server/ServerController.cpp
    src/server/ServerController.h

    src/server/ServerState.cpp
    src/server/ServerState.h

    src/logic/Message.cpp
    src/logic/Message.h
)

target_include_directories(server PRIVATE
    src/server
    src/logic
    C:/msys64/ucrt64/include
)

target_link_libraries(server PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Gui
    ws2_32
    mswsock
)
