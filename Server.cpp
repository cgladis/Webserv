//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

//Server::Server(int port, std::string ipAddress): qlen(5), exit(false) {
Server::Server(): qlen(5), exit(false) {
//    listeningSocket.bind(conf.GetPort(), conf.GetIP());
//    std::cout << "Start server http://" << conf.GetIP() << ":"
//        << conf.GetPort() << "/" << std::endl;
}

void Server::addServers(AllConfigs configs) {

	for (size_t i = 0; i < configs.size(); ++i) {
		Socket sock = Socket();

		//parse data from configs into socket
		sock.setConfig(configs[i]);

		//make socket ready to work
		sock.bind(configs[i].getPort(), configs[i].getIP());
		sock.listen(qlen);
		listeningSockets.push_back(sock);

		std::cout << "Start server http://" << configs[i].getIP() << ":"
				  << configs[i].getPort() << "/" << std::endl;
	}
}

int Server::mySelect(fd_set *readfds, fd_set *writefds) {
    struct timeval tv;
    int max_fd;
    int resSelect;

    int listeningSocketFd;
	int sessionFd;

	tv.tv_sec = 3;
	tv.tv_usec = 0;
    max_fd = 0;

	// make FD_SETS empty
    FD_ZERO(readfds);
    FD_ZERO(writefds);

	// filling readfds and writefds with all available fds in webServer
    for (size_t i = 0; i < listeningSockets.size(); ++i) {
        listeningSocketFd = listeningSockets[i].get_fd();
        FD_SET(listeningSocketFd, readfds);
        max_fd = std::max(listeningSocketFd, max_fd);
    }
	for (size_t i = 0; i < clients.size(); ++i) {
		sessionFd = clients[i].get_fd();
		FD_SET(sessionFd, readfds);
		FD_SET(sessionFd, writefds);
		max_fd = std::max(sessionFd, max_fd);
	}

	// waiting for incoming connection. Readfds and writefds will have fds, which we have connection with
    resSelect = select(max_fd + 1, readfds, writefds, NULL, &tv);

	std::cout << readfds << std::endl;

	// fdseesion = accept();
    //        resSelect = select(max_fd + 1, &readfds, &writefds, NULL, &tv);
    return (resSelect);
}

void Server::answerSocket() {

}

void Server::run() {

    fd_set readfds, writefds;
    int resSelect;

    while (!exit)
    {
        resSelect = mySelect(&readfds, &writefds);
        std::cout << "SELECT OK " << resSelect << "\n";
        if (resSelect < 0) {
            if (errno != EINTR) {
				// select error handling
				std::cout << "STATUS: ERROR " << std::endl;
			} else {
				// new signal
				std::cout << "STATUS: NO SIGNAL " << std::endl;
            }
            continue; //no events
        }
		if (resSelect == 0) {
			//time out
			std::cout << "STATUS: TIME OUT " << std::endl;
            continue;
        }
        for (size_t i = 0; i < listeningSockets.size(); ++i) {
            if (FD_ISSET(listeningSockets[i].get_fd(), &readfds)) {
                std::cout << "STATUS: CONNECT " << std::endl;
                connect(listeningSockets[i]); // connect event handling
            }
        }

        for (size_t i = 0; i < clients.size(); ++i) {
			if (FD_ISSET(clients[i].get_fd(), &readfds)){
				std::cout << "STATUS: OPEN FOR READ " << clients[i].get_fd() <<std::endl;
				clients[i].getRequest();
			}
			if (FD_ISSET(clients[i].get_fd(), &writefds)){
				std::cout << "STATUS: OPEN FOR WRITE " << clients[i].get_fd() <<std::endl;
				if (clients[i].areRespondReady()) {
					clients[i].sendAnswer();
					clients.erase(clients.begin() + i);
				}
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
    if (fd > 0){
        clients.push_back(Session(fd, inputSocket));
    }
}
