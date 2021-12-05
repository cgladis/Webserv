//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Config.hpp"
#include "Session.hpp"
#include <iostream>
#include <vector>

class Server {
public:
    Server(int port, std::string ipAddress);

    void init();
    void run();
    void connect();
//    void getRequest();

private:
    int mySelect(fd_set *readfds, fd_set *writefds);
    Socket listeningSocket;
    int qlen;
    std::vector<Session> clients;
    bool exit;
};


#endif
