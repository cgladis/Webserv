//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Socket.hpp"

Socket::Socket() {

	fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "New Socket. FD = " << fd << std::endl;

}

void Socket::bind(int port) {

	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(port);
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr), sizeof(socketAddr)) == -1)
        throw std::runtime_error("Bind error!");
    std::cout << "bind Ok" << std::endl;
}

void Socket::bind(int port, IPAddress ipAddress) {
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(port);
	socketAddr.sin_addr.s_addr = ipAddress.inet_addr();
    if (::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr), sizeof(socketAddr)) == -1)
        throw std::runtime_error("Bind error!");
    std::cout << "bind Ok, port: " << socketAddr.sin_port << std::endl;
}

void Socket::listen(int qlen) {
	if (::listen(fd, qlen) != 0)
		throw std::runtime_error("Error listen");
}



Socket::~Socket() {
	close(fd);

}

int Socket::getfd() {
	return fd;
}
