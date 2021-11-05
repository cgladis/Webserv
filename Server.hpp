//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Config.hpp"
#include <iostream>

class Server {
public:
    Server(int port, std::string ipAddress);

    void run();
    void processingRequests();

private:
    Socket ListeningSocket;
    int qlen;
};


#endif
