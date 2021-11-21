//
// Created by Александр Шоршин on 05.11.2021.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include <sys/socket.h> //sockaddr
#include <iostream>
#include <sstream>
#include <fstream>

#define BUFF_SIZE 100

class Session {
public:
    Session(int fd, sockaddr socket);

    void getRequest();
    void sendAnswer();
    int get_fd() const;

private:
    int fd;
    sockaddr socket;
    std::string request;

};


#endif
