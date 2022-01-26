//
// Created by Александр Шоршин on 05.11.2021.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include <sys/socket.h>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <string>
#include "AllConfigs.hpp"
#include "Socket.hpp"
#include "ErrorException.hpp"
#include <map>
#include <sys/stat.h>
#include <cstdio>
#include <string>
#include "StringArray.hpp"
#define C_RED "\033[31m"
#define C_WHITE "\033[0m"
#define C_BLUE "\033[34m"
#define C_YELLOW "\033[33m"
#define C_GREEN "\033[32m"
#define C_MAGENTA "\033[35m"

#define READING_BUFF 100

class Session {
public:
    Session(int fd, const Socket &);
	Session &operator=(const Session& oth);
    ~Session();

	void makeAndSendResponse(int fd, const std::string& response_body, unsigned int code = 200, const std::string
	&status = "OK", std::string response_header = "", bool nobody= false);
	void parseAsChunked();
	void parseHeader();
    void getRequest(const AllConfigs &configs);
	Location getMyLocation(const std::vector<Location> &locations, const std::string &url);
	void sendAnswer(char **env);
	void handleAsDir();
	void errorPageHandle(unsigned int);
    int get_fd() const;
    bool areRespondReady() const;
	void handleAsCGI(char **env);
	void handlePutAndPostRequest();
	void handleDeleteRequest();
	std::string openAndReadTheFile(const std::string &);
	void initializeAndCheckData();

private:
    int fd;
    bool respondReady;
    std::string request;
	std::string path;
	Config config;
	Location location;
	Socket sesSocket;
	std::map<std::string, std::string> header;
//	std::map<std::string, std::string> argsForCgi;
    std::string argsForCgi;
	std::string uploadedFilename;
	std::string fileText;
	bool isChunked;
	int contentLength;
	bool isHeaderRead;
	bool isSGI;
	long bodySum;
    int cgi_fd[2];
};


#endif
