#include <QApplication>
#include <memory>

#include "gui/LoginWindow.h"
#include "logic/ChatController.h"
#include "network/TcpNetworkClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::shared_ptr<INetworkClient> network = std::make_shared<TcpNetworkClient>();
    std::shared_ptr<ChatController> controller = std::make_shared<ChatController>(network);

    LoginWindow window(controller);
    window.show();

    return app.exec();
}
