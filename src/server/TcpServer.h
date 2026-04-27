#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "ServerController.h"

class TcpServer {
public:
    using tcp = boost::asio::ip::tcp;

    TcpServer(boost::asio::io_context& ioContext, unsigned short port);

private:
    void acceptNext();

    tcp::acceptor acceptor;
    ServerController controller;
};
