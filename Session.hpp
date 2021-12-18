//
// Created by Александр Шоршин on 05.11.2021.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include <sys/socket.h> //sockaddr
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <string>
#include "AllConfigs.hpp"
#include "Socket.hpp"
#include <map>

#define BUFF_SIZE 100

class Socket;

class Session {
public:
    Session(int fd, const Socket &socket);
    ~Session();

	void parseRequest();
    void getRequest();
    void sendAnswer();

    int get_fd() const;
    bool areRespondReady() const;

private:
    int fd;
    bool respondReady;
    std::string request;

	Socket socket;
	// keys (method, path, content-length, connection,
	std::map<std::string, std::string> header;
};


#endif
