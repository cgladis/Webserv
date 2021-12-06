//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

//Server::Server(int port, std::string ipAddress): qlen(5), exit(false) {
Server::Server(): qlen(5), exit(false) {
//    listeningSocket.bind(conf.GetPort(), conf.GetIP());
//    std::cout << "Start server http://" << conf.GetIP() << ":"
//        << conf.GetPort() << "/" << std::endl;
}

void Server::ADDServer(Config conf) {

    Socket listeningSocket;

    listeningSocket.bind(conf.GetPort(), conf.GetIP());
    listeningSockets.push_back(listeningSocket);

    std::cout << "Start server http://" << conf.GetIP() << ":"
              << conf.GetPort() << "/" << std::endl;
}

void Server::init() {
    for (int i = 0; i < listeningSockets.size(); ++i) {
        listeningSockets[i].listen(qlen);
    }
//    listeningSocket.listen(qlen);
}

int Server::mySelect(fd_set *readfds, fd_set *writefds) {
//    struct timeval tv;
    int max_fd;
    int resSelect;

    int listeningSocketFd;

//        tv.tv_sec = 1;
//        tv.tv_usec = 0;
    max_fd = 0;
    FD_ZERO(readfds);
    FD_ZERO(writefds);
    for (int i = 0; i < listeningSockets.size(); ++i) {
        listeningSocketFd = listeningSockets[i].get_fd();
        FD_SET(listeningSocketFd, readfds);
        max_fd = std::max(listeningSocketFd, max_fd);
    }
    for (int i = 0; i < listeningSockets.size(); ++i) {
        listeningSockets[i].fillReadfdsAndWritefds(readfds, writefds, &max_fd);
    }
    resSelect = select(max_fd + 1, readfds, writefds, NULL, NULL);
    //        resSelect = select(max_fd + 1, &readfds, &writefds, NULL, &tv);
    return (resSelect);
}

void Server::answerSocket() {

}

void Server::run() {


    fd_set readfds, writefds;
    int resSelect;

    while (!exit)
    {
        resSelect = mySelect(&readfds, &writefds);
        std::cout << "SELECT OK " << resSelect << "\n";
        if (resSelect < 0){
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

