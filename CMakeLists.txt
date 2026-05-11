cmake_minimum_required(VERSION 3.16)

project(CampusConnect VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Multimedia
    MultimediaWidgets
)

find_package(GTest REQUIRED)

find_path(BOOST_ASIO_INCLUDE_DIR
    NAMES boost/asio.hpp
    PATHS
        C:/msys64/ucrt64/include
        C:/msys64/mingw64/include
)

if(NOT BOOST_ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "Boost.Asio headers were not found. Install Boost or set BOOST_ASIO_INCLUDE_DIR.")
endif()

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

    src/gui/resources.qrc
)

target_link_libraries(app PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
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

target_link_libraries(server PRIVATE
    Qt6::Core
    Qt6::Network
)

target_include_directories(server PRIVATE
    ${BOOST_ASIO_INCLUDE_DIR}
)

if(WIN32)
    target_link_libraries(server PRIVATE
        ws2_32
        mswsock
    )
endif()

add_executable(final_tests_client
    final_tests/CLIENT/test_ChatController.cpp

    src/logic/ChatController.cpp
    src/logic/ChatController.h

    src/logic/ChatState.cpp
    src/logic/ChatState.h

    src/logic/InputValidator.cpp
    src/logic/InputValidator.h

    src/logic/Message.cpp
    src/logic/Message.h
)

target_include_directories(final_tests_client PRIVATE
    src/logic
    src/network
    final_tests/CLIENT
)

target_link_libraries(final_tests_client PRIVATE
    Qt6::Core
    Qt6::Network

    GTest::gtest
    GTest::gtest_main

    GTest::gmock
    GTest::gmock_main
)

add_executable(final_tests_server
    final_tests/SERVER/test_ChatState.cpp

    src/logic/ChatState.cpp
    src/logic/ChatState.h

    src/logic/Message.cpp
    src/logic/Message.h
)

target_include_directories(final_tests_server PRIVATE
    final_tests/SERVER
)

target_link_libraries(final_tests_server PRIVATE
    Qt6::Core

    GTest::gtest
    GTest::gtest_main
)

enable_testing()

add_test(NAME FinalClientTests COMMAND final_tests_client)
add_test(NAME FinalServerTests COMMAND final_tests_server)

if(WIN32)
    set_tests_properties(FinalClientTests FinalServerTests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:C:\\Qt\\Tools\\mingw1310_64\\bin;PATH=path_list_prepend:C:\\Qt\\6.10.2\\mingw_64\\bin;PATH=path_list_prepend:C:\\msys64\\mingw64\\bin;PATH=path_list_prepend:C:\\msys64\\ucrt64\\bin"
    )
endif()
