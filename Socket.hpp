//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <sys/socket.h> //socket
#include <netinet/in.h> //sockaddr_in
#include <unistd.h> //close
#include <fcntl.h> //fcntl
#include "IPAddress.hpp"
#include "Config.hpp"

class Socket
{
public:
	Socket();
    Socket(const Socket&);
    const Socket &operator = (const Socket&);
	~Socket();


	void bind(int port, IPAddress ipAddress);
    void listen(int qlen);
    int get_fd() const;
    void setConfig(const Config& conf);
	Config getConfig() const;

private:
	int fd;
    Config config;
};


#endif
