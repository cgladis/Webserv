//
// Created by Александр Шоршин on 04.11.2021.
//

#include <arpa/inet.h>
#include "Socket.hpp"
#include "cerrno" // TODO delete

Socket::Socket() {
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error while creating socket: " + std::string(std::strerror(errno))); // TODO delete
	std::cout << "New Socket. FD = " << fd << std::endl;
}

Socket::Socket(const Socket &other) {
    *this = other;
}

Socket const &Socket::operator=(const Socket &other) {
    this->fd = other.fd;
	this->locIp = other.locIp;
	this->locPort = other.locPort;
    return *this;
}

Socket::~Socket() {
}

void Socket::bind(const std::string &ip, int port) {
	this->locIp = ip;
	this->locPort = port;
	//create sockaddr
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(port);
	socketAddr.sin_addr.s_addr = ::inet_addr(ip.c_str());

	//cancel locked port
	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //nonblock mode
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	//bind port
    if (::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr), sizeof(socketAddr)) == -1) {
		std::cout << "Bind error! (Socket::bind) " + std::string(std::strerror(errno)) << std::endl; // TODO delete
		throw std::runtime_error("bind error");
	}
}

void Socket::listen(int qlen) {
	if (::listen(fd, qlen) != 0)
		throw std::runtime_error("Error listen");
}


int Socket::get_fd() const {
	return fd;
}

int Socket::getPort() const {
	return locPort;
}

std::string Socket::getIP() const {
	return locIp;
}


