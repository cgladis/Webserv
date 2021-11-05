//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

Server::Server(int port, std::string ipAddress): qlen(5) {

    ListeningSocket.bind(port, ipAddress);

}

void Server::run() {
    ListeningSocket.listen(qlen);
}

void Server::processingRequests() {
    int fd;
    sockaddr inputSocket;
    socklen_t len;
    fd = accept(ListeningSocket.getfd(), &inputSocket, &len);
    if (fd < 0)
        throw std::runtime_error("Error accept");
}
