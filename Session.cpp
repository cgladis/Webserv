//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"
std::vector<std::string> split(const std::string& s, char delimiter);
std::string formPath(const Location &location, const std::string &url);

Session::Session(int fd, const Socket& sock): fd(fd), sesSocket(sock) {
//    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

void Session::parseRequest() {
	std::stringstream ss(request);
	std::string curLine;
	std::string curWord;
	std::string value;

	std::getline(ss, curLine);
	std::stringstream localSs(curLine);
	std::string firstLineHeader[3] = {"Method:", "Path:", "HttpVersion:"};
	for (size_t i = 0; i < 3; ++i) {
		localSs >> curWord;
		header.insert(std::make_pair(firstLineHeader[i], curWord));
	}

	size_t startPos;
	int count = 0;
	while (std::getline(ss, curLine)) {
		if (curLine == "\r") {
			count++;
			continue;
		}
		if (count == 2) {
			fileText.append(curLine + "\n");
			continue;
		}
		startPos = curLine.find(':');
		value = curLine.substr(startPos + 2);
		header.insert(std::make_pair(curLine.substr(0, startPos + 1), value));
	}
}

//считывает запрос порциями по BUFF_SIZE
//если запрос считан до конца, ставит флаг respondReady
void Session::getRequest() {
	char buff[BUFF_SIZE + 1];
	ssize_t length = 0;

	length = recv(fd, &buff, BUFF_SIZE, 0);
    std::cout << "-------------------------------" << std::endl;
    std::cout << "FD: " << fd << " LENGTH: " << length << std::endl;
    std::cout << buff << std::endl;
    std::cout << "-------------------------------" << std::endl;
	if (length == 0){
		throw std::runtime_error("connection is closed");
    }
	else if (length < 0 ){
        // запрос еще не поступил
        return;
		throw std::runtime_error("receiving info error");
    }
    else if (length <= BUFF_SIZE) {

        buff[length] = 0;
        request.append(buff);

        char tmp[1];
        ssize_t check_eof = recv(fd, &tmp, 1, MSG_PEEK);
        if (check_eof < 0) {
            respondReady = true;
            std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
            std::cout << C_RED << request << C_WHITE << std::endl;
            parseRequest();
        }
    }
    else {
        throw std::runtime_error("unknown error");
    }
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
	std::ifstream erPage;
	exit(-1);
}

void makeAndSendResponse(int fd, int code, const std::string& response_body) {
	std::stringstream response;
	response << "HTTP/1.1 " << code << " OK\n"
			 << "Connection: close\n"
			 << "Content-Type: text/html; image/gif\n"
			 << "Content-Length: " << response_body.length() <<"\n"
			 << "\n"
			 << response_body;
    std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
    std::cout << C_BLUE << response.str() << C_WHITE << std::endl;
	int length = (int)send(fd, response.str().c_str(), response.str().length(), 0);
	if (length < 0)
		throw std::runtime_error("error sending data");
	else if (length == 0)
		throw std::runtime_error("error");
}

void Session::handleAsDir(const std::string &url) {
	if (access(path.c_str(), R_OK) == 1)
		errorPageHandle(403);
	DIR* dir = opendir(path.c_str());
	struct dirent* stDir;

	std::stringstream response_body;
	response_body << "<!DOCTYPE html>\n"
					 "<html lang=\"en\">\n"
					 "<head>\n"
					 "    <link rel=\"shortcut icon\" href=\"images/favicon.ico\">\n"
					 "    <meta charset=\"UTF-8\">\n"
					 "    <title>Title</title>\n"
					 "</head>\n"
					 "<body>\n"
					 "<ul>\n";
	while ((stDir = readdir(dir)) != nullptr) {
		if (url == "/")
			response_body << "<li><a href=\"" << url + stDir->d_name << "\">" << stDir->d_name << "</a></li>\n";
		else if (std::string(stDir->d_name) == ".")
			response_body << "<li><a href=\"" << url << "\">" << stDir->d_name << "</a></li>\n";
		else
			response_body << "<li><a href=\"" << url + "/" + stDir->d_name << "\">" << stDir->d_name << "</a></li>\n";
	}
	response_body << "</ul>\n"
					 "</body>\n"
					 "</html>\n";

	makeAndSendResponse(fd, 200, response_body.str());
	if (closedir(dir) == -1)
		throw std::runtime_error("error closing directory");
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
	std::string response;
	makeAndSendResponse(fd, 200, response_body.str());
}

//делает body ответа и отправяет на сокет
void Session::handleAsCGI() {

	// переменные класса
	// fd - фд сессии
	// path - путь к скрипту (.py)
	// header - мапа с данными из хедера запроса, чтобы получить данные, используй ее вот так header.at("Host:")
	// посмотреть содержимое мапы - либо через дебагер, либо разкоментить блок в SendAnswer
	// config - конфиг, с которым мы работаем на время текущего соединения
	// location - соответственно location, с которым мы работаем

    std::stringstream response_body;

    std::ifstream fin(path);
    if (!fin.is_open())
        throw std::runtime_error("file wasn't opened");

    std::string line;
    while (std::getline(fin, line)) {
        response_body << line;
        if (!fin.eof())
            response_body << "\n";
    }

    makeAndSendResponse(fd, 200, response_body.str());
}

//формирует и отправляет ответ
void Session::sendAnswer(const AllConfigs &configs) {
//	std::cout << std::endl << "MAP'S CONTENT";
//	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
//		std::cout << it->first << it->second << std::endl;

	std::string url = header.at("Path:");

	config = configs.getRightConfig(header.at("Host:"), sesSocket);
	Location location;
	if (!config.getLocations().empty())
		location = getMyLocation(config.getLocations(), url);
	if (!location.isMethodAvailable(header.at("Method:")))
		errorPageHandle(405);
	path = formPath(location, url);


	if (header.at("Method:") == "POST")
		handlePostRequest(location);
	else if (header.at("Method:") == "DELETE")
		handleDeleteRequest();

	//for checking is path a directory or a file
	struct stat st = {};
	stat(path.c_str(), &st);
	// checking the file is a dir, or a file
	if (S_ISREG(st.st_mode))
		handleAsFile();
	else if (S_ISDIR(st.st_mode)) {
		if (!location.getExec().empty() && location.getExec().substr(location.getExec().size() - 3) == ".py") {
			path.append(location.getExec());
			handleAsCGI();
		} else if (!location.getIndex().empty()) {
			path.append(location.getIndex());
			handleAsFile();
		} else if (location.isAutoIndex()) {
			handleAsDir(url);
		} else
			return;
		//no index, no autoindex and it is directory! error
	}
	else
		errorPageHandle(404);
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

Session &Session::operator=(const Session &oth) {
	this->fd = oth.fd;
    this->respondReady = oth.respondReady;
	return *this;
}

void Session::handlePostRequest(const Location &location) {
	if ((unsigned int)std::stoi(header.at("Content-Length:")) > location.getMaxBody())
		errorPageHandle(413);
	size_t pos;

	std::map<std::string, std::string>::iterator it;
	if ((it = header.find("Content-Disposition:")) != header.end()) {
		// extracting filename
		if ((pos = it->second.find("filename=")) != std::string::npos)
			uploadedFilename = it->second.substr(pos + 10);
		size_t start = uploadedFilename.find_first_not_of('\"');
		size_t end = uploadedFilename.find_last_not_of("\"\r");
		uploadedFilename = uploadedFilename.substr(start, end + 1);

		// defining, where file should be saved (if user defined)
		if (!location.getUploadStore().empty() && location.getUploadStore() != "/")
			uploadedFilename = location.getUploadStore() + "/" + uploadedFilename;

		// creating and writing to a file
		std::ofstream fileToUpl(uploadedFilename);
		if (fileToUpl.is_open())
			fileToUpl << fileText;
		else
			//filepath not found
		fileToUpl.close();
	}
}

void Session::handleDeleteRequest() {
	std::string fileToDelete = header.at("Path:").substr(1);
	std::remove(fileToDelete.c_str());
	path = "www/index.html";
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
		tokens.push_back(token);
	return tokens;
}

std::string formPath(const Location &location, const std::string &url) {
	std::string path;
	path = location.getRoot();
	if ((url == location.getLocationName() || url == location.getLocationName() + "/"))
		path.append("/");
	else if (url.size() > location.getLocationName().size()) {
		if (location.getLocationName() == "/")
			path.append("/");
		path.append(url.substr(location.getLocationName().size()));
	}
	return path;
}