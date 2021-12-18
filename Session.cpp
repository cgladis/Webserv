//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"

Session::Session(int fd, sockaddr socket): fd(fd), socket(socket) {
    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

Config findConfigByPort(AllConfigs configs, const std::string &port) {
	for (size_t i = 0; i < configs.size(); ++i)
		if (std::to_string(configs[i].getPort()) == port)
			return configs[i];
	return Config();
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

void Session::sendAnswer(const AllConfigs &configs) {
	(void)socket;
	Config conf = findConfigByPort(configs, port);

	// go through all paths in each location and find the same to path from request (if no -> 404)
	// check if request method does exist in location method vector (if no -> ???)
	// make new string = root + index and try to
	// 		if (method == GET)
	// 			open it for read and return to client (if file wasn't found) -> 404
	//		if (method == DELETE)
	//			delete it (if file wasn't found) -> 404
	//		if (method == POST)
	//			add data to server or execve(exec from location) ??



	if (path == "/")
		path = "www/index.html";
	else if (path == "/images/favicon.ico")
		path = "www/images/favicon.ico";
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
				 << "Host: localhost:8000\n" // does it mean client's ip:port?
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
