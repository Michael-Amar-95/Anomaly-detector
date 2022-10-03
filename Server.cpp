
#include "Server.h"

Server::Server(int port) throw(const char *) {
    stopSockets = false;
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
        throw "failure in socket";
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if (::bind(socketFD, (struct sockaddr *) &server, sizeof(server)) < 0)
        throw "failure in binding";

    if (listen(socketFD, 3) < 0)
        throw "failure in listen";
}

void Server::start(ClientHandler &ch) throw(const char *) {
    t = new thread([&ch, this]() {
        while (!stopSockets) {
            socklen_t clientSize = sizeof(client);
            alarm(3);
            int someClient = accept(socketFD, (struct sockaddr *) &client, &clientSize);
            if (someClient < 0)
                throw "failure in accept";
            ch.handle(someClient);
            close(someClient);
            sleep(2);
        }
        close(socketFD);
    });
}

void Server::stop() {
    stopSockets = true;
    t->join(); // do not delete this!
}

Server::~Server() {
    delete t;
}

