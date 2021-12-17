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

#define BUFF_SIZE 100

class Session {
public:
    Session(int fd, sockaddr socket);
    ~Session();

    void getRequest();
    void sendAnswer();
    void sendShortAnswer() const;
	void receiveFromClient();

    int get_fd() const;
    bool areRespondReady();

private:
    int fd;
    bool respondReady;
    sockaddr socket;
    std::string request;


};


#endif
