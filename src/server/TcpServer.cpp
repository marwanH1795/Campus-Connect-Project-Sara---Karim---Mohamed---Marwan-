#include "TcpServer.h"
#include "ClientSession.h"
#include "ServerController.h"

#include <cstdio>

TcpServer::TcpServer(boost::asio::io_context& ioContext, unsigned short port)
    : acceptor(ioContext, tcp::endpoint(tcp::v4(), port)),
      controller(std::make_shared<ServerController>())
{
    std::printf("Boost.Asio server running on port %hu\n", port);
    acceptNext();
}

void TcpServer::acceptNext() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<ClientSession>(
                std::move(socket),
                controller
            )->start();
        } else {
            std::printf("Accept error: %s\n", ec.message().c_str());
        }

        acceptNext();
    });
}
