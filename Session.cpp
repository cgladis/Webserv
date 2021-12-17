//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"

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
	else {
		std::cout << request << std::endl;
		respondReady = true;
	}
}

void Session::getRequest() {
    (void)socket;
	receiveFromClient();
}

int Session::get_fd() const {
    return fd;
}

void Session::sendAnswer() {

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

void Session::sendShortAnswer() const {
    std::stringstream response;
    response << "HTTP/1.1 200 OK\n"
             << "Host: localhost:8000\n"
             << "Content-Type: text/html; charset=UTF-8\n"
             << "Connection: close\n"
             << "Content-Length: " << 0 <<"\n"
             << "\n";
    send(fd, response.str().c_str(), response.str().length(), 0);
}

bool Session::areRespondReady() {
    return respondReady;
}

Session::~Session() {

//    close(fd);
//    std::cout << "CLOSE " << fd << std::endl;
    std::cout << "DESTRUCTOR " << fd << std::endl;

}
