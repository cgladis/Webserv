//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <sys/socket.h> //socket
#include <netinet/in.h> //sockaddr_in
#include <unistd.h> //close
#include "IPAddress.hpp"


class Socket
{

public:
	Socket();
	~Socket();

	void bind(int port);
	void bind(int port, IPAddress ipAddress);
    void listen(int qlen);
    int getfd();

private:
	int fd;

};


#endif
