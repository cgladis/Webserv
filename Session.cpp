//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"

Session::Session(int fd, const Socket &socket): fd(fd), socket(socket) {
    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

void Session::parseRequest() {
	std::stringstream ss(request);
	std::string curWord;

	for (int i = 0; std::getline(ss, curWord, ' '); ++i) {
		if (i == 0)
			header.insert(std::pair<std::string, std::string>("method", curWord));
		if (i == 1)
			header.insert(std::pair<std::string, std::string>("path", curWord));
		// need to parse all data
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
		std::cout << request << std::endl;
	}
	parseRequest();
}

void Session::sendAnswer() {
	Config config = socket.getConfig();

	// go through all paths in each location and find the same to path from request (if no -> 404)
	// check if request method does exist in location method vector (if no -> ???)
	// make new string = root + index and try to
	// 		if (method == GET)
	// 			open it for read and return to client (if file wasn't found) -> 404
	//		if (method == DELETE)
	//			delete it (if file wasn't found) -> 404
	//		if (method == POST)
	//			add data to server or execve(exec from location) ??

//	sendAnswerOnGetRequest();

	std::string path;

	if (header.at("path") == "/")
		path = "www/index.html";
	else if (header.at("path") == "/images/favicon.ico")
		path = "www/images/favicon.ico";
	std::cout << path << std::endl;
    std::ifstream fin(path); // path cannot start with '/'
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
				 << "Host: localhost:8000\n" // does it mean client's ip:port?
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
