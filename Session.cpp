//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"
#include "AllConfigs.hpp"


std::vector<std::string> split(const std::string& s, char delimiter);
std::string formPath(const Location &location, const std::string &url);
std::string fixPath(std::string &strToFix);
std::map<std::string, std::string> getArgsFromEncodedString(const std::string &str);
std::map<std::string, std::string> getTypesFromFile(const char *pathToFile);

Session::Session(int fd, const Socket &sock):
		fd(fd), respondReady(false), need_to_read_cgi(false), sesSocket(sock), isChunked(false), contentLength(-1), isHeaderRead(false),
		isSGI(false), bodySum(0), answer_sent(false), request_received(false){
    start_time = std::clock();
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
	std::string toParse;
	mimeTypes = getTypesFromFile("www/mimeTypes.txt");

	if (url.find('?') == url.size() - 1)
		url.pop_back();
	if (url.find('?') <= 0 && header.at("Method:") == "GET") {

        argsForCgiString = url.substr(url.find('?') + 1);
//		argsForCgi = getArgsFromEncodedString(toParse);
	}
	else if (header.at("Method:") == "POST" && header.at("Content-Type:") == "application/x-www-form-urlencoded\r")
		argsForCgi = getArgsFromEncodedString(fileText);

	location = getMyLocation(config.getLocations(), url);
	uploadedFilename = location.getRoot() + '/'
			+ location.getUploadStore() + '/'
			+ url.substr(location.getLocationName().size());
	fixPath(uploadedFilename);
	path = formPath(location, url);
	if (header.find("Content-Length:") != header.end() &&
		location.isMaxBody() &&
		atoi(header.at("Content-Length:").c_str()) > (int)location.getMaxBodySize())
		throw ErrorException(413);
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
            request_received = true;
			std::cout << C_MAGENTA << "FD " << fd <<" : 1 request_received = true" << C_WHITE << std::endl;
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
		usleep(2000);
	}
	else if (contentLength != -1) {
		recv(fd, &buff, 1, 0);
		char extraBuff[contentLength + 2];
		recv(fd, &extraBuff, contentLength, 0);
		extraBuff[contentLength] = 0;
		request.append("\n").append(extraBuff);
		fileText.append(extraBuff);
		respondReady = true;
        request_received = true;
		std::cout << C_MAGENTA << "FD " << fd <<" : 2 request_received = true" << C_WHITE << std::endl;
	}
	else {
        request_received = true;
		respondReady = true;
		std::cout << C_MAGENTA << "FD " << fd <<" : 3 request_received = true" << C_WHITE << std::endl;
	}
    std::cout << C_YELLOW << "FD " << fd << " :" << C_WHITE << std::endl;
    std::cout << C_RED << request << C_WHITE << std::endl;
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
&status, std::string response_header, bool nobody) {
	unsigned long ind;
	std::string key;

	std::stringstream response;
	response << "HTTP/1.1 " << code << " " << status << "\n";
	if (code == 301) {
        response << "Location: http://" << response_body << std::endl;
        nobody = true;
    }
	else if (code == 201) {
        response << "Content-Location: /" << path << std::endl;
        nobody = true;
    }
	else {
		response << "Connection: close" << "\n";
		ind = path.find_last_of('.');
		key = path.substr(ind + 1);
		std::map<std::string, std::string>::iterator it = mimeTypes.find(key);
		if (it != mimeTypes.end())
			response << "Content-Type: " << it->second << "\n";
		response << "Content-Length: " << response_body.length() << "\n";
	}
    response << response_header << "\n\n" << (nobody ? "": response_body);

    std::cout << C_YELLOW << "FD: " << fd << C_WHITE << std::endl;
    std::cout << C_BLUE << response.str() << C_WHITE << std::endl;
	ssize_t length = send(fd, response.str().c_str(), response.str().length(), 0);
    answer_sent = true;
    std::cout << C_GREEN << "FD " << fd << " answer_sent = true" << C_WHITE <<std::endl;
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

StringArray cgi_env(std::map<std::string, std::string> header, std::string path, Config conf, std::string argsForCgi, char **env)
{
    StringArray	tmp;
    std::string str;


    (void )header;
    (void )path;
    (void )argsForCgi;
    (void )env;
	tmp.addString("Auth_Type=Basic");
    tmp.addString("Gateway_Interface=CGI/1.1");
    tmp.addString("SCRIPT_FILENAME=" + path);
    tmp.addString("Server_Software=webserver");
    tmp.addString("REQUEST_METHOD=" + header.at("Method:"));
    tmp.addString("REDIRECT_STATUS=CGI");
//    tmp.addString("AllowEncodedSlashes=ON");
    if (header.find("Cookie:") != header.end())
        tmp.addString("HTTP_COOKIE=" + header.at("Cookie:"));

    // Добавление переменных из header c префиксом HTTP
    std::map<std::string, std::string>::iterator	begin = header.begin(), end = header.end();
    for (; begin != end; ++begin) {
		std::string locStr = begin->first;
        if (!locStr.empty())
        {
            locStr.pop_back();
            tmp.addString("HTTP_" + locStr + "=" + begin->second);
        }
    }

    // Добавление внешних переменных окружения
    while (env && *env)
    {
        str = static_cast<std::string>(*env);
        unsigned long ind = str.find('=');
        if (ind != 0)
            tmp.addString(str);
        env++;
    }

    // Добавление аргументов при GET запросе
//    if (header.at("Method:") == "GET")
//        tmp.addString("QUERY_STRING=" + argsForCgi);

    return tmp;
    (void )conf;
}

//делает body ответа и отправяет на сокет
void Session::handleAsCGI(char **env) {

    // переменные класса
    // fd - фд сессии
    // path - путь к скрипту (.py)
    // header - мапа с данными из хедера запроса, чтобы получить данные, используй ее вот так header.at("Host:")
    // посмотреть содержимое мапы - либо через дебагер, либо разкоментить блок в SendAnswer
    // config - конфиг, с которым мы работаем на время текущего соединения
    // location - соответственно location, с которым мы работаем


	StringArray cgi_env_map = cgi_env(header, path, config, argsForCgiString, env);
//    std::cout << cgi_env_map << std::endl;

//    std::cout << C_YELLOW << "ЗАШЕЛ  FD "  << fd << C_WHITE <<std::endl;
    std::ifstream fin(path);
    if (!fin.is_open())
        throw std::runtime_error("file wasn't opened");

    if (header.at("Method:") == "POST"){
        pipe(cgi_fd_in);
        pipe(cgi_fd_out);
        pid = fork();
        if (pid == 0){
            std::cout << C_GREEN << "before execve " << path.c_str() << C_WHITE << std::endl;
            close(cgi_fd_out[1]);
            dup2(cgi_fd_out[0], 0);
            dup2(cgi_fd_in[1], 1);
            execve(path.c_str(), NULL, cgi_env_map.c_Arr());
//            execve(path.c_str(), NULL, NULL);
            std::cout << C_RED << "ОШИБКА" << C_WHITE << std::endl;
            std::cout << strerror(errno) <<std::endl;
            std::cout << C_GREEN << "after execve" <<  C_WHITE << std::endl;
            close(cgi_fd_out[0]);
            close(cgi_fd_in[0]);
            close(cgi_fd_in[1]);
            exit(400);
        }
        close(cgi_fd_in[1]);
        write(cgi_fd_out[1], fileText.c_str(), fileText.size());
        close(cgi_fd_out[0]);
        close(cgi_fd_out[1]);


    }
    else if (header.at("Method:") == "GET"){
        pipe(cgi_fd_in);
        pid = fork();
        if (pid == 0){
            std::cout << C_GREEN << "before execve " << path.c_str() << C_WHITE << std::endl;
            dup2(cgi_fd_in[1], 1);
            execve(path.c_str(), NULL, cgi_env_map.c_Arr());
//            execve(path.c_str(), NULL, NULL);
            std::cout << C_RED << "ОШИБКА " << C_WHITE << strerror(errno) <<std::endl;
            std::cout << C_GREEN << "after execve" <<  C_WHITE << std::endl;
            close(cgi_fd_in[0]);
            close(cgi_fd_in[1]);
            exit(400);
        }
        close(cgi_fd_in[1]);

    }
    else{
        throw std::runtime_error("can't execute this method for cgi");
    }

    std::cout << C_YELLOW << "FD " << fd << " open pid: " << pid << C_WHITE << std::endl;

    need_to_read_cgi = true;
    respondReady = false;
    std::cout << "need_to_read_cgi = true" << std::endl;

}

void Session::read_cgi() {

    if (not need_to_read_cgi) return;

    int status;
    pid_t res_wait = waitpid(pid, &status, WNOHANG);
//    std::cout << C_RED << "FD " << fd << " : wait pid: " << pid << " ;res_wait:  " << res_wait << C_WHITE << std::endl;
    if (res_wait <= 0)
        return;
    if (status != 0)
        throw ErrorException(status);
    std::cout << C_YELLOW << "FD " << fd << " : wait pid: " << pid << " ; status:  " << status << C_WHITE << std::endl;
    std::stringstream cgi_response;

    char data_buf[READING_BUFF + 1];
    ssize_t data_length;
    while ((data_length = read(cgi_fd_in[0], &data_buf, READING_BUFF)) > 0) {
//        std::cout << data_buf << " : " << data_length << std::endl;
        data_buf[data_length] = '\0';
        cgi_response << data_buf;
        std::cout << C_MINT << data_buf << C_WHITE;
    }

    if (cgi_response.str().size() == 0){
        std::cout << C_RED << "read_cgi res_read=" << data_length << " errno=" << errno << " - " << std::strerror(errno) << std::endl;
        return;
    }

    need_to_read_cgi = false;
    close(cgi_fd_in[0]);

    std::string cgi_response_str = cgi_response.str();

//    std::cout << cgi_response_str << std::endl;

    std::string response_header = cgi_response_str.substr(0,cgi_response_str.find("\n\n"));
    std::string response_body = cgi_response_str.substr(cgi_response_str.find("\n\n")+2);

    std::cout << C_YELLOW << "HEAD: \n" << C_WHITE << response_header << std::endl;
    std::cout << C_YELLOW << "BODY: \n" << C_WHITE << response_body << std::endl;


    makeAndSendResponse(fd, response_body, 200, "OK", response_header);
}

void readUslessRest(int fd) {
	char buff[2];
	while (recv(fd, &buff, 1, 0) == 1)
		continue;
}

void Session::sendAnswer(char **env) {
	readUslessRest(fd);
	if (config.getIsReturn()) {
        std::cout << C_GREEN << "sendAnswer 1" << C_WHITE << std::endl;
        makeAndSendResponse(fd, config.getReturnField(), config.getReturnCode(), "Moved Permanently");}
	else if ((path.substr(path.size() - 3) == ".py"
			  || path.substr(path.size() - 4) == ".php" || path.substr(path.size() - 3) == ".sh")
			  && (header.at("Method:") == "POST" || header.at("Method:") == "GET")
			  && access(path.c_str(), 2) == 0) {
        std::cout << C_GREEN << "sendAnswer 2" << C_WHITE << std::endl;
		handleAsCGI(env);}
	else if (header.at("Method:") == "POST") {
        std::cout << C_GREEN << "sendAnswer 3" << C_WHITE << std::endl;
        handlePostRequest(env);}
	else if (header.at("Method:") == "PUT"){
        std::cout << C_GREEN << "sendAnswer 4" << C_WHITE << std::endl;
		handlePutRequest();}
	else if (header.at("Method:") == "DELETE"){
        std::cout << C_GREEN << "sendAnswer 5" << C_WHITE << std::endl;
		handleDeleteRequest();}
	else{
        std::cout << C_GREEN << "sendAnswer 6" << C_WHITE << std::endl;
		handleGetRequest(env);}
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
    this->need_to_read_cgi = oth.need_to_read_cgi;
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
	this->argsForCgi = oth.argsForCgi;
	this->mimeTypes = oth.mimeTypes;
    this->start_time = oth.start_time;
    this->answer_sent = oth.answer_sent;
    this->request_received = oth.request_received;
	return *this;
}



void Session::handlePostRequest(char **env) {
	std::ofstream ofile;
	std::string fileName;
	std::string fileBody;
	std::string line;

	std::string uploadPath = location.getRoot() + '/' + location.getUploadStore() + '/';
	fixPath(uploadPath);


	if (header.at("Content-Type:").find("multipart/form-data") != std::string::npos) {
		fileName = fileText.substr(fileText.find("filename=") + 10);
		fileName = fileName.substr(0, fileName.find_first_of('\"'));
		ofile.open(uploadPath + fileName, std::ios_base::out | std::ios_base::trunc);
		if (!ofile.is_open())
			throw ErrorException(404);
		fileBody = fileText.substr(fileText.find("fileName=") + 10);
		std::stringstream body(fileBody);
		for (int i = 0; i < 4; i++)
			std::getline(body, line);
		while (std::getline(body, line)) {
			if (line == "\r")
				break;
			ofile << line << "\n";
		}
		ofile.close();
	}
	handleGetRequest(env);
}

void Session::handleDeleteRequest() {
	if (access(path.c_str(), 0) != 0)
		throw ErrorException(403);
	std::remove(path.c_str());
	makeAndSendResponse(fd, "file " + path + " deleted");
}

void Session::handlePutRequest() {
	std::ofstream ofile(uploadedFilename, std::ios_base::out | std::ios_base::trunc);
	if (ofile.is_open())
		ofile << fileText;
	ofile.close();
	makeAndSendResponse(fd, std::to_string(fileText.size()));
}

void Session::handleGetRequest(char **env) {
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
		} else if (!location.getExec().empty()) {
			path.append("/" + location.getExec());
			fixPath(path);
			handleAsCGI(env);
		}
		else
			throw ErrorException(500);
	}
	else
		throw ErrorException(404);
}

int Session::time_passed() const {
    return     int((clock() - start_time) / CLOCKS_PER_SEC);
}

bool Session::are_need_to_read_cgi() const {
    return need_to_read_cgi;
}

bool Session::is_answer_sent() const {
    return answer_sent;
}

bool Session::no_request() const {
    return request.empty();
}

bool Session::is_request_received() const {
    return request_received;
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

std::map<std::string, std::string> getArgsFromEncodedString(const std::string &str) {
	std::map<std::string, std::string> mapToReturn;
	unsigned long index;
	std::vector<std::string> argsArray = split(str, '&');
	for (std::vector<std::string>::iterator it = argsArray.begin(); it != argsArray.end(); it++) {
		index = it->find('=');
		mapToReturn.insert(std::make_pair<std::string, std::string>(it->substr(0, index), it->substr(index + 1)));
	}
	return mapToReturn;
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

std::map<std::string, std::string> getTypesFromFile(const char *pathToFile) {
	std::map<std::string, std::string> types;
	std::ifstream ifile(pathToFile);
	std::string line;
	std::string key;
	std::string value;
	if (!ifile.is_open())
		throw ErrorException(500);
	while (std::getline(ifile, line)) {
		std::stringstream ss(line);
		ss >> value >> key;
		types.insert(std::pair<std::string, std::string>(key, value));
	}
	ifile.close();
	return types;
}
