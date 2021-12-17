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

void Server::addServers(AllConfigs configs) {

	for (size_t i = 0; i < configs.size(); ++i) {
		Socket sock = Socket();

		//parse data from configs into socket
		sock.setConfig(configs[i]);

		//make socket ready to work
		sock.bind(configs[i].getPort(), configs[i].getIP());
		sock.listen(qlen);
		FD_SET(sock.get_fd(), &masReadFds);
		listeningSockets.push_back(sock);

		std::cout << "Start server http://" << configs[i].getIP() << ":"
				  << configs[i].getPort() << "/" << std::endl;
	}
}

int Server::mySelect() {
	// usleep for clean and copy fds properly
	FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	usleep(2000);
	readFds = masReadFds;
	writeFds = masWriteFds;
	usleep(2000);
    return select(getMaxFd() + 1, &readFds, &writeFds, nullptr, nullptr);
}

void Server::answerSocket() {

}


void Server::run() {
    int resSelect;

    while (!exit)
    {
        resSelect = mySelect();
		std::cout << "SELECT : OK = " << resSelect << std::endl;

		// if error occurred
		if (resSelect <= 0) {
			handleSelectError(resSelect);
			continue;
		}

        for (size_t i = 0; i < listeningSockets.size(); ++i)
            if (FD_ISSET(listeningSockets[i].get_fd(), &readFds)) {
                std::cout << "STATUS: CONNECT " << std::endl;
                connect(listeningSockets[i]); // connect event handling
            }

        for (size_t i = 0; i < sessions.size(); ++i) {
			if (FD_ISSET(sessions[i].get_fd(), &readFds)){
				std::cout << "STATUS: OPEN FOR READ " << sessions[i].get_fd() <<std::endl;
				sessions[i].getRequest();
			}
			if (FD_ISSET(sessions[i].get_fd(), &writeFds) && sessions[i].areRespondReady()) {
				std::cout << "STATUS: OPEN FOR WRITE " << sessions[i].get_fd() <<std::endl;
				sessions[i].sendAnswer(listeningSockets);
				finishSession(i);
			}
		}
    }
}

void Server::connect(const Socket &currentSocket) {

    //try get request
    int fd;
    sockaddr inputSocket;
    socklen_t len;
    fd = accept(currentSocket.get_fd(), &inputSocket, &len);
    if (fd > 0) {
		FD_SET(fd, &masReadFds);
		FD_SET(fd, &masWriteFds);
        sessions.push_back(Session(fd, inputSocket));
	}
	else
		std::cout << "ACCEPT ERROR. Cannot create a connection" << std::endl;
}

void Server::finishSession(size_t i) {
	FD_CLR(sessions[i].get_fd(), &masReadFds);
	FD_CLR(sessions[i].get_fd(), &masWriteFds);
	close(sessions[i].get_fd());
	sessions.erase(sessions.begin() + i);
}
