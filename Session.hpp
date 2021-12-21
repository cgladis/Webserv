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
    Session(int fd, Socket &socket);
	Session &operator=(const Session& oth);
    ~Session();

	void parseRequest();
    void getRequest();
    void sendAnswer();

	const Socket &getSocket();
    int get_fd() const;
    bool areRespondReady() const;

private:
    int fd;
    bool respondReady;
    std::string request;

	Socket &socket;
	std::map<std::string, std::string> header;
};


#endif
