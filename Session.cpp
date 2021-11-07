//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"

Session::Session(int fd, sockaddr socket): fd(fd), socket(socket) {
    std::cout << "Add client: " << fd << std::endl;
}

void Session::getRequest() {

    (void)socket;
    char buff[BUFF_SIZE + 1];
    unsigned long length;

    length = recv(fd, buff, BUFF_SIZE, 0);
    if (length > 0) {
        buff[length] = 0;
        request += buff;
    } else {
        std::cout << "fd: " << fd << std::endl;
        std::cout << "message " << request.length() << " byte:" << std::endl << request;
        request.clear();
    }

}

int Session::get_fd() const{
    return fd;
}

void Session::sendAnswer() {

}
