#include <boost/asio/io_context.hpp>
#include <iostream>

#include "src/server/TcpServer.h"

int main() {
    try {
        boost::asio::io_context ioContext;

        TcpServer server(ioContext, 12345);

        ioContext.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}
