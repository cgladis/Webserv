//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"

Session::Session(int fd, sockaddr socket): fd(fd), socket(socket) {
    std::cout << "Add client: " << fd << std::endl;
    sendAnswer();
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
//        sendAnswer();
    }

}

int Session::get_fd() const{
    return fd;
}

void Session::sendAnswer() {

    std::stringstream response;
    response << "HTTP/1.1 200 OK\n"
             << "Host: localhost:8000\n"
             << "Content-Type: text/html; charset=UTF-8\n"
             << "Connection: close\n"
             << "Content-Length: 21\n"
             << "\n" << "<h1>CONNECT</h1>";

    std::ifstream fin("./www/index.html");
    std::string line;
    while (std::getline(fin, line))
    {
        response << line;
        if (!fin.eof())
            response << "\n";
    }

//    std::cout << response.str() << std::endl;

    send(fd, response.str().c_str(), response.str().size(), 0);

}
