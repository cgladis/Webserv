//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

Server::Server(int port, std::string ipAddress): qlen(5), exit(false) {

    listeningSocket.bind(port, ipAddress);

}

void Server::init() {
    listeningSocket.listen(qlen);
}

void Server::run() {

    fd_set readfds, writefds;
    int resSelect;
    int max_fd;

    while (!exit)
    {
        max_fd = listeningSocket.get_fd();
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(listeningSocket.get_fd(), &readfds);
        for (size_t i = 0; i < clients.size(); ++i) {
            FD_SET(clients[i].get_fd(), &readfds);
            if (clients[i].get_fd() > max_fd)
                max_fd = clients[i].get_fd();
        }
        resSelect = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (resSelect < 1){
            if (errno != EINTR){
                // select error handling
            } else{
                // new signal
            }
            continue; //no events
        }
        if (resSelect == 0) {
            //time out
            continue;
        }
        if (FD_ISSET(listeningSocket.get_fd(), &readfds)){
            connect(); // connect event handling
        }
        for (size_t i = 0; i < clients.size(); ++i) {
            if (FD_ISSET(clients[i].get_fd(), &readfds)){
                clients[i].getRequest();
            }
        }
        getRequest();
    }

}

void Server::connect() {

    //try get request
    int fd;
    sockaddr inputSocket;
    socklen_t len;
    fd = accept(listeningSocket.get_fd(), &inputSocket, &len);
    if (fd > 0){
        clients.push_back(Client(fd, inputSocket));
    }
}

void Server::getRequest() {

    for (size_t i = 0; i < clients.size(); ++i) {
        clients[i].getRequest();
    }

}
