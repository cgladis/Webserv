//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Session.hpp"
#include "AllConfigs.hpp"
#include "FDSet.hpp"
#include <iostream>
#include <vector>

class Server {
public:
    Server();

    void addServers(const AllConfigs&);
    void run(const AllConfigs &, char **env);
    void connect(Socket &currentSocket);
	void finishSession(size_t i);

private:
    std::vector<Socket> listeningSockets;
    int qlen;
    bool exit;
    std::vector<Session> sessions;
    FDSet fds;
};


#endif
