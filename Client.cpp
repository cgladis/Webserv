//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Client.hpp"

Client::Client(int fd, sockaddr socket): fd(fd), socket(socket) {
    std::cout << "Add client: " << fd << std::endl;
}

void Client::getRequest() {

    (void)socket;
    char buff[100];
    unsigned long length;

    length = recv(fd, buff, 100, 0);
    if (length > 0) {
        buff[length] = 0;
        request += buff;
    } else {
        std::cout << "fd: " << fd << std::endl;
        std::cout << "message " << request.length() << " byte:" << std::endl << request;
        request.clear();
    }

}

int Client::get_fd() const{
    return fd;
}
