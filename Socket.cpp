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
//	if (!inet_aton("127.0.0.1", &socketAddr.sin_addr)){
//		std::cout << "incorrect ip" << std::endl;
//	}
	std::cout << "bind result = "
			<< ::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr),
					  sizeof(socketAddr)) << std::endl;
}

void Socket::bind(int port, IPAddress ipAddress) {
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(port);
	socketAddr.sin_addr.s_addr = ipAddress.inet_addr();
//	if (!inet_aton("127.0.0.1", &socketAddr.sin_addr)){
//		std::cout << "incorrect ip" << std::endl;
//	}
	std::cout << "bind result = "
			  << ::bind(fd, reinterpret_cast<const sockaddr *>(&socketAddr),
						sizeof(socketAddr)) << std::endl;
}



Socket::~Socket() {
	close(fd);

}