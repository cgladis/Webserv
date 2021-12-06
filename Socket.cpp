//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Socket.hpp"

Socket::Socket() {

	fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "New Socket. FD = " << fd << std::endl;

}

//void Socket::bind(int port) {
//
//	//create sockaddr
//	sockaddr_in socketAddr;
//	socketAddr.sin_family = AF_INET;
//	socketAddr.sin_port = htons(port);
//	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//	//delete locked port
//	int opt = 1;
//	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//
//	//bind port
//    if (::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr), sizeof(socketAddr)) == -1)
//        throw std::runtime_error("Bind error!");
//    std::cout << "bind Ok" << std::endl;
//}

void Socket::bind(int port, IPAddress ipAddress) {

	//create sockaddr
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(port);
	socketAddr.sin_addr.s_addr = ipAddress.inet_addr();

	//cancel locked port
	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //nonblock mode
//    int flags = fcntl(fd, F_GETFL);
//    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	//bind port
    if (::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr), sizeof(socketAddr)) == -1)
        throw std::runtime_error("Bind error! (Socket::bind)");
//    std::cout << "bind Ok, port: " << ntohs(socketAddr.sin_port) << std::endl;
}

void Socket::listen(int qlen) {
	if (::listen(fd, qlen) != 0)
		throw std::runtime_error("Error listen");
}



Socket::~Socket() {
	close(fd);

}

int Socket::get_fd() {
	return fd;
}

void Socket::addClient(Session client) {
    this->clients.push_back(client);
}

void Socket::fillReadfdsAndWritefds(fd_set *readfds, fd_set *writefds, int *max_fd) {
    for (size_t i = 0; i < clients.size(); ++i) {
        FD_SET(clients[i].get_fd(), readfds);
        FD_SET(clients[i].get_fd(), writefds);
        *max_fd = std::max(clients[i].get_fd(), *max_fd);
    }
}
