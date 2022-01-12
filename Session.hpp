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
#define BUFF_SIZE 100
#define C_RED "\033[31m"
#define C_WHITE "\033[0m"
#define C_BLUE "\033[34m"
#define C_YELLOW "\033[33m"

class Session {
public:
    Session(int fd, const Socket &);
	Session &operator=(const Session& oth);
    ~Session();

	void parseRequest();
    void getRequest();
    void sendAnswer(const AllConfigs &);
	void handleAsFile();
	void handleAsDir(const std::string &);
	void errorPageHandle(const int &);
    int get_fd() const;
    bool areRespondReady() const;
	void handleAsCGI();
	void handlePostRequest(const Location &);
	void handleDeleteRequest();

private:
    int fd;
    bool respondReady;
    std::string request;
	std::string path;
	Config config;
	Socket sesSocket;
	std::map<std::string, std::string> header;
	std::string uploadedFilename;
	std::string fileText;
};


#endif
