//
// Created by Александр Шоршин on 05.11.2021.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h> //sockaddr
#include <iostream>


class Client {
public:
    Client(int fd, sockaddr socket);

    void getRequest();
    int get_fd() const;

private:
    int fd;
    sockaddr socket;
    std::string request;

};


#endif
