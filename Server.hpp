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
    Server();

	int getMaxFd();
    void addServers(const AllConfigs&);
    void run(const AllConfigs &);
    void connect(Socket &currentSocket);
	void finishSession(size_t i);

private:
    int mySelect();
    std::vector<Socket> listeningSockets;
    int qlen;
    bool exit;
    std::vector<Session> sessions;
	fd_set writeFds, readFds, masWriteFds, masReadFds;
};


#endif
