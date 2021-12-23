//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

int Server::getMaxFd() {
	int max;
	for (unsigned int i = 0; i < listeningSockets.size(); ++i)
		max = max < listeningSockets[i].get_fd() ? listeningSockets[i].get_fd() : max;
	for (unsigned int i = 0; i < sessions.size(); ++i)
		max = sessions[i].get_fd() > max ? sessions[i].get_fd() : max;
	return max;
}

void handleSelectError(int resSelect) {
	if (resSelect < 0) {
		if (errno != EINTR)
			std::cout << "STATUS: ERROR " << std::endl;
		else
			std::cout << "STATUS: NO SIGNAL " << std::endl;
	}
	if (resSelect == 0)
		std::cout << "STATUS: TIME OUT " << std::endl;
}

Server::Server(): qlen(5), exit(false) {

}

void Server::addServers(const AllConfigs &configs) {
	// создаем столько сокетов, сколько уникальных Ip Port
	for (size_t i = 0; i < configs.getUniqIpPortVectorSize(); ++i) {
		Socket sock = Socket();

		std::string ipPort = configs.getIpPort(i);
		std::string ip = ipPort.substr(0, ipPort.find(':'));
		int port = std::atoi(ipPort.substr(ipPort.find(':') + 1).c_str());
		//make socket ready to work
		sock.bind(ip, port);
		sock.listen(qlen);
		FD_SET(sock.get_fd(), &masReadFds);
		listeningSockets.push_back(sock);

		std::cout << "Start server http://" << configs.getIpPort(i) << "/" << std::endl;
	}
}

int Server::mySelect() {
	// usleep for clean and copy fds properly
	FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	usleep(10000);
	readFds = masReadFds;
	writeFds = masWriteFds;
	usleep(10000);
    return select(getMaxFd() + 1, &readFds, &writeFds, nullptr, nullptr);
}

void Server::run(const AllConfigs &configs) {
    int resSelect;

    while (!exit)
    {
        resSelect = mySelect();
//		std::cout << "SELECT : OK = " << resSelect << std::endl;

		if (resSelect <= 0) {
			handleSelectError(resSelect);
			continue;
		}

        for (size_t i = 0; i < listeningSockets.size(); ++i)
            if (FD_ISSET(listeningSockets[i].get_fd(), &readFds)) {
                connect(listeningSockets[i]); // connect event handling
            }

        for (size_t i = 0; i < sessions.size(); ++i) {
			if (FD_ISSET(sessions[i].get_fd(), &readFds)){
//				std::cout << "STATUS: OPEN FOR READ " << sessions[i].get_fd() <<std::endl;
				sessions[i].getRequest();
			}
			if (FD_ISSET(sessions[i].get_fd(), &writeFds) && sessions[i].areRespondReady()) {
//				std::cout << "STATUS: OPEN FOR WRITE " << sessions[i].get_fd() <<std::endl;
				sessions[i].sendAnswer(configs);
				finishSession(i);
			}
		}
    }
}

void Server::connect(Socket &currentSocket) {

    //try get request
    int fd;
    sockaddr inputSocket;
    socklen_t len;
    fd = accept(currentSocket.get_fd(), &inputSocket, &len);
    if (fd > 0) {
		FD_SET(fd, &masReadFds);
		FD_SET(fd, &masWriteFds);
		// give some time to set fds in fd_sets
		usleep(20000);
        sessions.push_back(Session(fd, currentSocket));
	}
	else
		std::cout << "ACCEPT ERROR. Cannot create a connection" << std::endl;
}

void Server::finishSession(size_t i) {
	FD_CLR(sessions[i].get_fd(), &masReadFds);
	FD_CLR(sessions[i].get_fd(), &masWriteFds);
	usleep(10000);
	close(sessions[i].get_fd());
//	std::cout << "SESSION CLOSED. FD: " << sessions[i].get_fd() << std::endl;
	sessions.erase(sessions.begin() + i);
}
