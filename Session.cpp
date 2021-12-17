//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"

Session::Session(int fd, sockaddr socket): fd(fd), socket(socket) {
    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

void Session::receiveFromClient() {
	char buff[BUFF_SIZE + 1];
	unsigned long length;

	length = recv(fd, buff, BUFF_SIZE, 0);
	if (length < 0 )
		throw std::runtime_error("receiving info error");
	else if (length == BUFF_SIZE) {
		buff[length] = 0;
		request.append(buff);
	}
	else
		respondReady = true;
}

void Session::parseRequest() {
	std::stringstream ss(request);
	std::string uslessWord;
	for (int i = 0; i < 7; ++i) {
		if (i == 0)
			ss >> method;
		else if (i == 1)
			ss >> path;
		else if (i == 4) {
			ss >> uslessWord;
			port = uslessWord.substr(uslessWord.find(':') + 1);
		}
		else if (i == 6) {
			ss >> uslessWord;
			browser = uslessWord.substr(0, 7);
		}
		else
			ss >> uslessWord;
	}
}

void Session::getRequest() {
    (void)socket;
	receiveFromClient();
}

Config findConfigByPort(AllConfigs configs, const std::string &port) {
	for (size_t i = 0; i < configs.size(); ++i)
		if (std::to_string(configs[i].getPort()) == port)
			return configs[i];
	return Config();
}

void Session::sendAnswer(const AllConfigs &configs) {
	parseRequest();
	Config conf = findConfigByPort(configs, port);
	std::cout << conf.getIP().getIP() << std::endl;

    std::ifstream fin("./www/index.html");
    std::string line;
    std::stringstream response_body;
    while (std::getline(fin, line)) {
        response_body << line;
        if (!fin.eof())
            response_body << "\n";
    }

    std::stringstream response;
    response << "HTTP/1.1 200 OK\n"
             << "Host: localhost:8000\n"
             << "Content-Type: text/html; charset=UTF-8\n"
             << "Connection: close\n"
             << "Content-Length: " << response_body.str().length() <<"\n"
             << "\n"
             << response_body.str();

    send(fd, response.str().c_str(), response.str().length(), 0);

    request.clear();
    respondReady = false;

    close(fd);
    std::cout << "CLOSE " << fd << std::endl;

}

bool Session::areRespondReady() {
    return respondReady;
}

int Session::get_fd() const {
	return fd;
}

Session::~Session() {
    std::cout << "Session's " << fd << " was closed" << std::endl;
}
