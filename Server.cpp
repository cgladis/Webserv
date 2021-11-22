//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

Server::Server(int port, std::string ipAddress): qlen(5), exit(false) {

    listeningSocket.bind(port, ipAddress);
    std::cout << "Start server http://" << ipAddress << ":"
        << port << "/" << std::endl;

}

void Server::init() {
    listeningSocket.listen(qlen);
}

void Server::run() {

    struct timeval tv;
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
            FD_SET(clients[i].get_fd(), &writefds);
            if (clients[i].get_fd() > max_fd)
                max_fd = clients[i].get_fd();
        }
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        resSelect = select(max_fd + 1, &readfds, &writefds, NULL, NULL);
        std::cout << "SELECT OK " << resSelect << "\n";
        if (resSelect < 1){
            if (errno != EINTR){
                std::cout << "STATUS: ERROR " << std::endl;
                // select error handling
            } else{
                std::cout << "STATUS: NO SIGNAL " << std::endl;
                // new signal
            }
            continue; //no events
        }
        if (resSelect == 0) {
            std::cout << "STATUS: TIME OUT " << std::endl;
            //time out
            continue;
        }
        if (FD_ISSET(listeningSocket.get_fd(), &readfds)){
            std::cout << "STATUS: CONNECT " << std::endl;
            connect(); // connect event handling
        }
        for (size_t i = 0; i < clients.size(); ++i) {
            if (FD_ISSET(clients[i].get_fd(), &readfds)){
                std::cout << "STATUS: OPEN FOR READ " << clients[i].get_fd() <<std::endl;
                clients[i].getRequest();
            }
            if (FD_ISSET(clients[i].get_fd(), &writefds)){
                std::cout << "STATUS: OPEN FOR WRITE " << clients[i].get_fd() <<std::endl;
                if (clients[i].areRespondReady()) {
                    clients[i].sendAnswer();
                    clients.erase(clients.begin() + i);
                }
            }
        }
    }

}

void Server::connect() {

    //try get request
    int fd;
    sockaddr inputSocket;
    socklen_t len;
    fd = accept(listeningSocket.get_fd(), &inputSocket, &len);
    if (fd > 0){
        clients.push_back(Session(fd, inputSocket));
    }
}
