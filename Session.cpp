//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"


std::vector<std::string> split(const std::string& s, char delimiter);
std::string formPath(const Location &location, const std::string &url);
std::string fixPath(std::string &strToFix);

Session::Session(int fd, const Socket &sock):
		fd(fd), respondReady(false), sesSocket(sock), isChunked(false), contentLength(-1), isHeaderRead(false),
		isSGI(false), bodySum(0) {
}

void Session::parseHeader() {
	if (request.empty())
		throw std::runtime_error("print error");
	std::stringstream ss(request);
	std::string curLine;
	std::string curWord;
	size_t startPos;
	std::string value;

	std::getline(ss, curLine);
	std::stringstream localSs(curLine);
	std::string firstLineHeader[3] = {"Method:", "Path:", "HttpVersion:"};
	header.clear();
	for (size_t i = 0; i < 3; ++i) {
		localSs >> curWord;
		header.insert(std::make_pair(firstLineHeader[i], curWord));
	}
	while (std::getline(ss, curLine)) {
		startPos = curLine.find(':');
		value = curLine.substr(startPos + 2);
		header.insert(std::make_pair(curLine.substr(0, startPos + 1), value));
	}
	if (header.find("Content-Length:") != header.end())
		contentLength = std::atoi(header.at("Content-Length:").c_str());
	else if (header.find("Transfer-Encoding:") != header.end() && header.at("Transfer-Encoding:") == "chunked\r") {
		isChunked = true;
	}
	if (header.at("HttpVersion:") != "HTTP/1.1")
		throw ErrorException(505);
	recv(fd, nullptr, 1, 0);

}

int readLength(int fd) {
	std::string length;
	char buff[2];
	recv(fd, &buff, 1, 0);
	while (buff[0] != '\r') {
		buff[1] = 0;
		length.append(buff);
		recv(fd, &buff, 1, 0);
	}
	recv(fd, &buff, 1, 0);
	if (length == "0")
		return 0;
	int x;
	std::stringstream ss;
	ss << std::hex << length;
	ss >> x;
	return x;
}

void Session::parseAsChunked() {
	int chunkLen;

	chunkLen = readLength(fd);
	if (chunkLen == 0) {
		if (location.isMaxBody() &&
		bodySum > location.getMaxBodySize())
			throw ErrorException(413);
		respondReady = true;
		recv(fd, nullptr, 2, 0);
		return;
	}
	char megaBuff[chunkLen + 1];
	megaBuff[chunkLen] = 0;
	bodySum += recv(fd, &megaBuff, chunkLen, 0);
	fileText.append(megaBuff);
	recv(fd, &megaBuff, 2, 0);
}

void Session::initializeAndCheckData() {
	std::string url = header.at("Path:");

	location = getMyLocation(config.getLocations(), url);
	uploadedFilename = location.getUploadStore() + "/" + url.substr(location.getLocationName().size());
	fixPath(uploadedFilename);
	path = formPath(location, url);
	if (header.find("Content-Length:") != header.end() &&
		location.isMaxBody() &&
		atoi(header.at("Content-Length").c_str()) > (int)location.getMaxBodySize())
		throw ErrorException(413);
	if ((path.substr(path.size() - 4) == ".bla" || path.substr(path.size() - 4) == ".py"
		|| path.substr(path.size() - 4) == ".php" || path.substr(path.size() - 4) == ".sh")
		&& !location.getExec().empty()) {
		isSGI = true;
		return;
	}
	if (!location.isMethodAvailable(header.at("Method:")))
		throw ErrorException(405);
}

void Session::getRequest(const AllConfigs &configs) {
	char buff[2];
	if (request.find("\r\n\r") != std::string::npos && !isHeaderRead) {
		parseHeader();
		config = configs.getRightConfig(header.at("Host:"), sesSocket);
		if (config.getIsReturn()) {
			respondReady = true;
			return;
		}
		initializeAndCheckData();
		isHeaderRead = true;
	}
	else if (!isHeaderRead) {
		recv(fd, &buff, 1, 0);
		buff[1] = 0;
		request.append(buff);
		return;
	}

	if (isChunked) {
		parseAsChunked();
		usleep(1500);
	}
	else if (contentLength != -1) {
		char extraBuff[contentLength + 2];
		recv(fd, &extraBuff, contentLength + 1, 0);
		extraBuff[contentLength + 1] = 0;
		request.append(extraBuff);
		fileText = extraBuff;
		respondReady = true;
	}
	else {
		std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
		std::cout << C_RED << request << C_WHITE << std::endl;
		respondReady = true;
	}
}

Location Session::getMyLocation(const std::vector<Location> &locations, const std::string &url) {
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

void Session::errorPageHandle(unsigned int code) {
	std::ifstream errorFile;
	std::string status = code == 200 ? "OK" : code == 403 ? "Forbidden" : code == 404 ? "Not Found" :
			code == 405 ? "Method Not Allowed" : code == 411 ? "Length Required" :
			code == 413 ? "Request Entity Too Large": code == 500 ? "Internal Server Error" :
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
		  "\t<h1>" << status << "</h1>\n";
	if (code == 405)
		ss << "\t<h1> Available methods: " << location.getAvailableMethods() << "</h1>";
	ss << "</body>\n"
		  "</html>";
	makeAndSendResponse(fd, ss.str(), code, status);
}

void Session::makeAndSendResponse(int fd, const std::string& response_body, unsigned int code, const std::string
&status) {
	std::stringstream response;
	response << "HTTP/1.1 " << code << " " << status << "\n";
	if (code == 301)
		response << "Location: http://" << response_body << "\n"
				<< "\n"
				<< "";
	else if (code == 201)
		response << "Content-Location: /" << path << "\n"
				 << "\n"
				 << "";
	else {
		response << "Connection: close" << "\n"
				 << "Content-Type: text/html; image/gif;" << "\n"
				 << "Content-Length: " << response_body.length() << "\n"
				 << "\n"
				 << response_body;
	}
    std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
    std::cout << C_BLUE << response.str() << C_WHITE << std::endl;
	ssize_t length = send(fd, response.str().c_str(), response.str().length(), 0);
	if (length <= 0)
		throw ErrorException(500);
}

void Session::handleAsDir() {
	std::string pathToFile;
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
		pathToFile = path.substr(location.getRoot().size()) + "/" + stDir->d_name;
		fixPath(pathToFile);
		response_body << "<li><a href=\"" << pathToFile << "\">" << stDir->d_name << "</a></li>\n";
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

void Session::handleAsCGI() {
	makeAndSendResponse(fd, std::to_string(fileText.size()));
}

void Session::sendAnswer() {
	if (config.getIsReturn())
		makeAndSendResponse(fd, config.getReturnField(), config.getReturnCode(), "Moved Permanently");
	else if (isSGI && header.at("Method:") != "DELETE")
		handleAsCGI();
	else if (header.at("Method:") == "PUT" || header.at("Method:") == "POST")
		handlePutAndPostRequest();
	else if (header.at("Method:") == "DELETE")
		handleDeleteRequest();
	else {
		struct stat st = {};
		stat(path.c_str(), &st);
		if (S_ISREG(st.st_mode))
			makeAndSendResponse(fd,  openAndReadTheFile(path));
		else if (S_ISDIR(st.st_mode)) {
			if (!location.getIndex().empty()) {
				path.append("/" + location.getIndex());
				fixPath(path);
				makeAndSendResponse(fd,  openAndReadTheFile(path));
			} else if (location.isAutoIndex()) {
				handleAsDir();
			} else
				throw ErrorException(500);
		}
		else
			throw ErrorException(404);
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
	this->location = oth.location;
	this->sesSocket = oth.sesSocket;
	this->header = oth.header;
	this->uploadedFilename = oth.uploadedFilename;
	this->fileText = oth.fileText;
	this->isChunked = oth.isChunked;
	this->contentLength = oth.contentLength;
	this->isHeaderRead = oth.isHeaderRead;
	this->isSGI = oth.isSGI;
	this->bodySum = oth.bodySum;
	return *this;
}

void Session::handlePutAndPostRequest() {
	std::cout << request << std::endl;
	if (header.at("Method:") == "POST" && access(path.c_str(), 2) != 0)
		throw ErrorException(403);
	if (uploadedFilename[0] == '/')
		uploadedFilename = uploadedFilename.substr(1);
	std::ofstream ofile(uploadedFilename, std::ios_base::out | std::ios_base::trunc);
	if (ofile.is_open()) {
		ofile << fileText;
		makeAndSendResponse(fd, std::to_string(fileText.size()));
		ofile.close();
	}
	else
		makeAndSendResponse(fd, fileText);
}

void Session::handleDeleteRequest() {
	if (access(path.c_str(), 0) != 0)
		throw ErrorException(403);
	std::remove(path.c_str());
	makeAndSendResponse(fd, "file " + path + " deleted");
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
	path.append("/");
	if (url.size() > location.getLocationName().size())
		path.append(url.substr(location.getLocationName().size()));

	return fixPath(path);
}

std::string fixPath(std::string &strToFix) {
	std::string readyPath;
	unsigned long i = -1;
	while (++i < strToFix.size()) {
		readyPath.push_back(strToFix[i]);
		if (strToFix.at(i) == '/') {
			while (strToFix[i + 1] == '/')
				i++;
		}
	}
	strToFix = readyPath;
	return readyPath;
}
