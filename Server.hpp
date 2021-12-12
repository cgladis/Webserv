//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Session.hpp"
#include "AllConfigs.hpp"
#include <iostream>
#include <vector>

class Server {
public:
//    Server(int port, std::string ipAddress);
    Server();

    void addServers(AllConfigs);
    void run();
    void connect(const Socket &currentSocket);
//    void getRequest();

private:
    int mySelect(fd_set *readfds, fd_set *writefds);

//    void addClient(Session client);

    std::vector<Socket> listeningSockets;
    int qlen;
    bool exit;
    void answerSocket();
    std::vector<Session> clients;
};


#endif
