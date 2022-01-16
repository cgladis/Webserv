//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"
std::vector<std::string> split(const std::string& s, char delimiter);
std::string formPath(const Location &location, const std::string &url);


Session::Session(int fd, const Socket &sock): fd(fd), sesSocket(sock) {
//    std::cout << "New session: " << fd << std::endl;
    respondReady = false;
}

void Session::parseRequest() {
	if (request.empty()) {
		throw std::runtime_error("print error");
	}
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
		ssize_t length;

		length = recv(fd, &buff, BUFF_SIZE, 0);

//    std::cout << "-------------------------------" << std::endl;
//    std::cout << "FD: " << fd << " LENGTH: " << length << std::endl;
//    std::cout << buff << std::endl;
//    std::cout << "-------------------------------" << std::endl;

		if (length == 0) {
			respondReady = true;
			std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
			std::cout << C_RED << request << C_WHITE << std::endl;
			parseRequest();
			throw std::runtime_error("connection is closed");
		}
		else if (length < 0 ) {
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

void Session::errorPageHandle(unsigned int &code) {
	std::ifstream errorFile;
	std::string status = code == 200 ? "OK" : code == 404 ? "Not Found" : code == 403 ? "Forbidden" : code == 405 ?
			"Method Not Allowed" : code == 413 ? "Request Entity Too Large": code == 500 ? "Internal Server Error" :
			code == 503 ? "Service Unavailable" : code == 505 ? "HTTP Version Not Supported" : "";

	for (unsigned long i = 0; i < config.getErrorPagesVectorSize(); ++i) {
		if (code == config.getCode(i)) {
			makeAndSendResponse(fd,  openAndReadTheFile(config.getPath(i)), code, status);
			return;
		}
	}
	// default error page if wasn't defined in .conf
	std::stringstream ss;
	ss << "<!DOCTYPE html>\n"
		  "<html lang=\"EN\">\n"
		  "<head>\n"
		  "\t<link rel=\"shortcut icon\" href=\"../images/favicon.ico\">\n"
		  "\t<meta charset=\"UTF-8\">\n"
		  "\t<title>Error Page</title>\n"
		  "\n"
		  "</head>\n"
		  "<body>\n"
		  "\t<h1>" << code << "</h1>\n"
		  "\t<h1>" << status << "</h1>\n"
		  "</body>\n"
		  "</html>";
	makeAndSendResponse(fd, ss.str(), code, status);
}

void Session::makeAndSendResponse(int fd, const std::string& response_body, unsigned int code, const std::string
&status) {
	std::stringstream response;
	response << "HTTP/1.1 " << code << " " << status << "\n"
			 << "Connection: close" << "\n"
			 << "Content-Type: text/html; image/gif;" << "\n"
			 << "Content-Length: " << response_body.length() <<"\n"
			 << "\n"
			 << response_body;
    std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
    std::cout << C_BLUE << response.str() << C_WHITE << std::endl;
	ssize_t length = send(fd, response.str().c_str(), response.str().length(), 0);
	if (length <= 0)
		throw ErrorException(500);
}

void Session::handleAsDir(const std::string &url) {
	if (access(path.c_str(), R_OK) == 1)
		throw ErrorException(403);
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

	makeAndSendResponse(fd, response_body.str());
	if (closedir(dir) == -1)
		throw ErrorException(500);
}

std::string Session::openAndReadTheFile(const std::string &filename) {
	path = filename;
	if (access(path.c_str(), R_OK) == 1)
		throw ErrorException(403);
//	if (path.substr(path.size() - 4) != ".html" && path.substr(path.size() - 3) != ".ico")
//		throw ErrorException(400);
	std::ifstream fin(path);
	if (!fin.is_open())
		throw ErrorException(404);
	std::string line;
	std::stringstream response_body;
	while (std::getline(fin, line)) {
		response_body << line;
		if (!fin.eof())
			response_body << "\n";
	}
	return response_body.str();
}

//делает body ответа и отправяет на сокет
void Session::handleAsCGI() {
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
    makeAndSendResponse(fd, response_body.str());
}

//формирует и отправляет ответ
void Session::sendAnswer(const AllConfigs &configs) {
//	std::cout << std::endl << "MAP'S CONTENT";
//	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
//		std::cout << it->first << it->second << std::endl;
	try {
		if (header.at("HttpVersion:") != "HTTP/1.1")
			throw ErrorException(505);
		std::string url = header.at("Path:");
		config = configs.getRightConfig(header.at("Host:"), sesSocket);
		Location location;
		if (!config.getLocations().empty())
			location = getMyLocation(config.getLocations(), url);
		else
			throw ErrorException(500);
		if (!location.isMethodAvailable(header.at("Method:")))
			throw ErrorException(405);
		path = formPath(location, url);

		if (header.at("Method:") == "POST")
			handlePostRequest(location);
		else if (header.at("Method:") == "DELETE")
			handleDeleteRequest();

		// checking the file is a dir, or a file
		struct stat st = {};
		stat(path.c_str(), &st);
		if (S_ISREG(st.st_mode))
			makeAndSendResponse(fd,  openAndReadTheFile(path));
		else if (S_ISDIR(st.st_mode)) {
			if (!location.getExec().empty() && location.getExec().substr(location.getExec().size() - 3) == ".py") {
				path.append(location.getExec());
				handleAsCGI();
			} else if (!location.getIndex().empty()) {
				path.append(location.getIndex());
				makeAndSendResponse(fd,  openAndReadTheFile(path));
			} else if (location.isAutoIndex()) {
				handleAsDir(url);
			} else
				throw ErrorException(500);
		}
		else
			throw ErrorException(404);
	}
	catch (ErrorException &er) {
		errorPageHandle(er.error_code);
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

Session &Session::operator=(const Session &oth) {
	this->fd = oth.fd;
    this->respondReady = oth.respondReady;
    this->request = oth.request;
	this->path = oth.path;
	this->config = oth.config;
	this->sesSocket = oth.sesSocket;
	this->header = oth.header;
	this->uploadedFilename = oth.uploadedFilename;
	this->fileText = oth.fileText;
	return *this;
}

void Session::handlePostRequest(const Location &location) {
	if (header.find("Content-Length:") == header.end())
		throw ErrorException(405);
	if ((unsigned int)std::stoi(header.at("Content-Length:")) > location.getMaxBody())
		throw ErrorException(400);
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
		else {
			fileToUpl.close();
			throw ErrorException(403);
		}
		fileToUpl.close();
	}
	else
		throw ErrorException(400);
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