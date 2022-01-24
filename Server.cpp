//
// Created by Александр Шоршин on 04.11.2021.
//

#include "Server.hpp"

//инициализация сервера
//qlen - количество одновременных соединений
Server::Server(): qlen(5), exit(false) {

}

void handleSelectError(int resSelect) {
	if (resSelect < 0) {
		std::cout << "STATUS: ERROR " << std::endl;
	}
	if (resSelect == 0)
		std::cout << "STATUS: TIME OUT " << std::endl;
}

//открывает сокеты для каждого уникального Ip Port в config-файле.
//создаем столько сокетов, сколько уникальных Ip Port
void Server::addServers(const AllConfigs &configs) {
	for (size_t i = 0; i < configs.getUniqIpPortVectorSize(); ++i) {
		Socket sock = Socket();

		std::string ipPort = configs.getIpPort(i);
		std::string ip = ipPort.substr(0, ipPort.find(':'));
		int port = std::atoi(ipPort.substr(ipPort.find(':') + 1).c_str());
		//make socket ready to work
		sock.bind(ip, port);
		sock.listen(qlen);
        fds.addReadFD(sock.get_fd());
		listeningSockets.push_back(sock);

		std::cout << "Start server http://" << configs.getIpPort(i) << "/" << std::endl;
	}
}

//Запускает сервер.
//Открывает бесконечный цикл опроса наших сокетов и работу с ними
void Server::run(const AllConfigs &configs, char **env) {
    int resSelect;
	bool needToRestart;
	int g = 0;

    while (!exit)
    {
		g++;
		needToRestart = false;
        resSelect = fds.select();

		if (resSelect <= 0) {
			handleSelectError(resSelect);
			continue;
		}

        for (size_t i = 0; i < listeningSockets.size(); ++i) {
			if (fds.isSetReadFD(listeningSockets[i].get_fd())) {
				connect(listeningSockets[i]);
				needToRestart = true;
			}
		}
		if (needToRestart)
			continue;
		for (size_t i = 0; i < sessions.size(); ++i) {
			try {
				if (fds.isSetReadFD(sessions[i].get_fd())) {
					sessions[i].getRequest(configs);
				}
				if (fds.isSetWriteFD(sessions[i].get_fd()) && sessions[i].areRespondReady()) {
					sessions[i].sendAnswer(env);
					finishSession(i);
					g = 0;
				}
				if (fds.isSetWriteFD(sessions[i].get_fd()) && g > 5000) {
					finishSession(i);
					g = 0;
				}
			} catch (ErrorException &er) {
				sessions[i].errorPageHandle(er.error_code);
				finishSession(i);
			}
		}
	}
}

//устанавливает соединение с сокетом
//добавляет fd сокета в список опрашиваемых
//открывает новую сессию для сокета
void Server::connect(Socket &currentSocket) {

    int fd;
    sockaddr inputSocket;
    socklen_t len;
    fd = accept(currentSocket.get_fd(), &inputSocket, &len);
    if (fd > 0) {
        fds.addFD(fd);
        sessions.push_back(Session(fd, currentSocket));
	}
	else
		std::cout << "ACCEPT ERROR. Cannot create a connection" << std::endl;
}

void Server::finishSession(size_t i) {
//    std::cout << "FD " << sessions[i].get_fd() << " CLOSED" <<std::endl;
    fds.deleteFD(sessions[i].get_fd());
    std::cout << "FD " << sessions[i].get_fd() << " CLOSED" <<std::endl;
	close(sessions[i].get_fd());
//	std::cout << "SESSION CLOSED. FD: " << sessions[i].get_fd() << std::endl;
	sessions.erase(sessions.begin() + i);
}
