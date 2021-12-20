//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"

Session::Session(int fd, Socket &socket): fd(fd), socket(socket) {
    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

void Session::parseRequest() {
	std::stringstream ss(request);
	std::string curLine;
	std::string curWord;
	size_t startPos;

	std::getline(ss, curLine);
	std::stringstream localSs(curLine);
	std::string firstLineHeader[3] = {"Method:", "Path:", "HttpVersion:"};
	for (size_t i = 0; i < 3; ++i) {
		localSs >> curWord;
		header.insert(std::make_pair(firstLineHeader[i], curWord));
	}

	while (std::getline(ss, curLine)) {
		startPos = curLine.find(':');
		header.insert(std::make_pair(curLine.substr(0, startPos + 1), curLine.substr(startPos + 2)));
	}
}

void Session::getRequest() {
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
		respondReady = true;
		parseRequest();
	}
}

Location getMyLocation(const std::vector<Location> &locations, const std::string &url) {
	Location retLocation;
	for (size_t i = 0; i < locations.size(); ++i) {
		if (url.find(locations[i].getLocationName()) != std::string::npos)
			retLocation = locations[i];
	}
	return retLocation;
}

void Session::sendAnswer() {
	std::cout << std::endl << "MAP'S CONTENT" << std::endl;
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
		std::cout << it->first << it->second << std::endl;

	std::string path;
	std::string url = header.at("Path:");
	Config config = socket.getConfig();
//	Location location = getMyLocation(config.getLocations(), url);





    std::ifstream fin(path);
	if (!fin.is_open())
		exit(-1);
    std::string line;
    std::stringstream response_body;
    while (std::getline(fin, line)) {
        response_body << line;
        if (!fin.eof())
            response_body << "\n";
    }

    std::stringstream response;
	if (path == "www/index.html") {
		response << "HTTP/1.1 200 OK\n" // depends on parseRequest
				 << "Host: localhost:8000\n" // server_name from .conf
				 << "Content-Type: text/html; charset=UTF-8\n" // depends on path text/html for html, image/x-icon
				 << "Connection: close\n"
				 << "Content-Length: " << response_body.str().length() <<"\n"
				 << "\n"
				 << response_body.str();
	}
	else if (path == "www/images/favicon.ico") {
		response << "HTTP/1.1 200 OK\n" // depends on parseRequest
				 << "Host: localhost:8000\n" // does it mean client's ip:port? (server's ip:port)
				 << "Content-Type: image/avif\n" // depends on path text/html for html, image/x-icon
				 << "Connection: close\n"
				 << "Content-Length: " << response_body.str().length() <<"\n"
				 << "\n"
				 << response_body.str();
	}


    send(fd, response.str().c_str(), response.str().length(), 0);

}

bool Session::areRespondReady() const {
    return respondReady;
}

int Session::get_fd() const {
	return fd;
}

Session::~Session() {
    std::cout << "Session's " << fd << " was closed" << std::endl;
}

const Socket &Session::getSocket() {
	return socket;
}

Session &Session::operator=(const Session &oth) {
	this->fd = oth.fd;
	this->socket = oth.socket;
	return *this;
}
