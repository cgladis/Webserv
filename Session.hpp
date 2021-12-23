//
// Created by Александр Шоршин on 05.11.2021.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <string>
#include "AllConfigs.hpp"
#include "Socket.hpp"
#include <map>
#include <sys/stat.h>

#define BUFF_SIZE 100

class Session {
public:
    Session(int fd, const Socket &);
	Session &operator=(const Session& oth);
    ~Session();

	void parseRequest();
    void getRequest();
    void sendAnswer(const AllConfigs &);
	void handleAsFile();
//	void autoindexHandle();
	void errorPageHandle(const int &);
    int get_fd() const;
    bool areRespondReady() const;
	Config getConfig() const;
	void setConfig(const Config &);
	Socket getSocket() const;

private:
    int fd;
    bool respondReady;
    std::string request;
	std::string path;
	Config config;
	Socket sesSocket;
	std::map<std::string, std::string> header;
};


#endif
