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

	while (!exit) {
		resSelect = fds.select();

//        std::cout << fds << std::endl;

		if (resSelect <= 0) {
			handleSelectError(resSelect);
			continue;
		}
		for (size_t i = 0; i < listeningSockets.size(); ++i) {
			if (fds.isSetReadFD(listeningSockets[i].get_fd())) {
				connect(listeningSockets[i]);
			}
		}
		for (size_t i = 0; i < sessions.size(); ++i) {
			try {
                if (sessions[i].no_request() and (sessions[i].time_passed() > TIME_OUT_FOR_GET_REQUEST)) {
                    std::cout << C_RED << "FD " << sessions[i].get_fd() << " : TIMEOUT "
                              << sessions[i].time_passed() << " sec. Connection closed." << C_WHITE << std::endl;
                    finishSession(i);
                    continue;
                }
				if (fds.isSetReadFD(sessions[i].get_fd())) {
                    if (not sessions[i].is_request_received())
					    sessions[i].getRequest(configs);
				}
				if (fds.isSetWriteFD(sessions[i].get_fd())) {
                    if (sessions[i].areRespondReady() and not sessions[i].are_need_to_read_cgi()) {
//                        std::cout << "FD "<<sessions[i].get_fd()<<" areRespondReady()" << std::endl;

                        sessions[i].sendAnswer(env);
                        if (sessions[i].is_answer_sent()) {
                            finishSession(i);
                        }
                    }
                    else if (sessions[i].are_need_to_read_cgi()){

//                        std::cout << "sessions["<<sessions[i].get_fd()<<"].are_need_to_read_cgi()" << std::endl;
                        sessions[i].read_cgi();
                        if (sessions[i].is_answer_sent()) {
                            finishSession(i);
                        }
                    }
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
    sockaddr inputSocket = {};
    socklen_t len;

    fd = accept(currentSocket.get_fd(), &inputSocket, &len);
    if (fd > 0) {
        std::cout << C_GREEN << "FD " << fd << " : OPEN" << C_WHITE << std::endl;
        fds.addFD(fd);
        sessions.push_back(Session(fd, currentSocket));
	}
	else {
        throw std::runtime_error("ACCEPT ERROR. Cannot create a connection");
    }
}

void Server::finishSession(size_t i) {
//    std::cout << "FD " << sessions[i].get_fd() << " CLOSED" <<std::endl;
    fds.deleteFD(sessions[i].get_fd());
    std::cout << C_RED << "FD " << sessions[i].get_fd() << " CLOSED"<< C_WHITE <<std::endl;
	close(sessions[i].get_fd());
//	std::cout << "SESSION CLOSED. FD: " << sessions[i].get_fd() << std::endl;
	sessions.erase(sessions.begin() + i);
}
