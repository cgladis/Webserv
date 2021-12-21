//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"
std::vector<std::string> split(const std::string& s, char delimiter);

Session::Session(int fd, Socket &socket): fd(fd), socket(socket) {
//    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

void Session::parseRequest() {
//	std::cout << request << std::endl;
	std::stringstream ss(request);
	std::string curLine;
	std::string curWord;
	std::string localTheme;
	size_t startPos;

	std::getline(ss, curLine);
	std::stringstream localSs(curLine);
	std::string firstLineHeader[3] = {"Method:", "Path:", "HttpVersion:"};
	for (size_t i = 0; i < 3; ++i) {
		localSs >> curWord;
		header.insert(std::make_pair(firstLineHeader[i], curWord));
	}

	while (std::getline(ss, curLine)) {
		if (curLine.empty())
			break;
		startPos = curLine.find(':');
		localTheme = curLine.substr(startPos + 2);
		header.insert(std::make_pair(curLine.substr(0, startPos + 1), localTheme));
	}
}

void Session::getRequest() {
	char buff[BUFF_SIZE + 1];
	unsigned long length;

	length = recv(fd, buff, BUFF_SIZE, 0);
	if (length == 0)
		throw std::runtime_error("connection is closed");
	else if (length < 0 )
		throw std::runtime_error("receiving info error");
	else if (length == BUFF_SIZE || (length > 0 && length < BUFF_SIZE)) {
		buff[length] = 0;
		request.append(buff);
		respondReady = (length > 0 && length < BUFF_SIZE);
	}
	else
		throw std::runtime_error("unknown error");
}

Location getMyLocation(const std::vector<Location> &locations, const std::string &url) {
	if (url == "/")
		return locations[locations.size() - 1];
	std::vector<std::string> urlWords = split(url, '/');
	for (size_t i = 0; i < locations.size(); ++i) {
		std::vector<std::string> locationWords = split(locations[i].getLocationName(), '/');
		for (size_t l = 0; l < locationWords.size(); ++l)
			if (urlWords[l] == locationWords[l]) {
				if (l + 1 == locationWords.size())
					return locations[i];
				continue;
			}
			else
				break;
	}
	return locations[locations.size() - 1];
}

void Session::errorPageHandle(const int &code) {
	(void)code;
}

void Session::handleAsFile() {
	if (access(path.c_str(), R_OK) == 1)
		errorPageHandle(403);
	std::ifstream fin(path);
	if (!fin.is_open())
		throw std::runtime_error("file wasn't opened");
	std::string line;
	std::stringstream response_body;
	while (std::getline(fin, line)) {
		response_body << line;
		if (!fin.eof())
			response_body << "\n";
	}

	std::stringstream response;
	response << "HTTP/1.1 200 OK\n"
			 << "Connection: close"
			 << "Content-Type: text/html"
			 << "Content-Length: " << response_body.str().length() <<"\n"
			 << "\n"
			 << response_body.str();

	int length = (int)send(fd, response.str().c_str(), response.str().length(), 0);
	if (length < 0)
		throw std::runtime_error("error sending data");
	else if (length == 0)
		throw std::runtime_error("error");
}

void Session::sendAnswer() {
	parseRequest();
//	std::cout << "MAP'S CONTENT" << std::endl;
//	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
//		std::cout << it->first << it->second << std::endl;

	std::string url = header.at("Path:");
	Config config = socket.getConfig();
	Location location;
	if (!config.getLocations().empty())
		location = getMyLocation(config.getLocations(), url);

	path = location.getRoot();
	if (location.getLocationName() != "/")
		path.append(url.substr(location.getLocationName().size()));

	struct stat st = {};
	stat(path.c_str(), &st);
	if (S_ISREG(st.st_mode))
		handleAsFile();
	else if (S_ISDIR(st.st_mode)) {
		if (location.getIndex() != nullptr) {
			path.append(location.getIndex());
			handleAsFile();
		} else if (location.isAutoIndexOn()) {
			// list all files in directory
			// check access
		} else
			return;
		//no index, no autoindex and it is direcotry! error
	}
}

bool Session::areRespondReady() const {
    return respondReady;
}

int Session::get_fd() const {
	return fd;
}

Session::~Session() {
//    std::cout << "Session's " << fd << " was closed" << std::endl;
}

const Socket &Session::getSocket() {
	return socket;
}

Session &Session::operator=(const Session &oth) {
	this->fd = oth.fd;
	this->socket = oth.socket;
	return *this;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::string news;
	if (s[0] == '/')
		news = s.substr(1);
	else
		news = s;
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(news);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}
