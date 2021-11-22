//
// Created by Александр Шоршин on 05.11.2021.
//

#include "Session.hpp"

Session::Session(int fd, sockaddr socket): fd(fd), socket(socket) {
    std::cout << "Add client: " << fd << std::endl;
    respondReady = false;
//    sendAnswer();
}

void Session::getRequest() {

    (void)socket;
    char buff[BUFF_SIZE + 1];
    unsigned long length;

    length = recv(fd, buff, BUFF_SIZE, 0);
    if (length > 0) {
        buff[length] = 0;
        request += buff;
        if (length < BUFF_SIZE){
//            std::cout << "fd: " << fd << std::endl;
//            std::cout << "message " << request.length() << " byte:" << std::endl << request;
//            sendAnswer();
            respondReady = true;
//            close(fd);
        }
    } else {
//        std::cout << "fd: " << fd << std::endl;
//        std::cout << "message " << request.length() << " byte:" << std::endl << request;
        respondReady = true;
    }

}

int Session::get_fd() const{
    return fd;
}

void Session::sendAnswer() {

    std::ifstream fin("./www/index.html");
    std::string line;
    std::stringstream response_body;
    while (std::getline(fin, line))
    {
        response_body << line;
        if (!fin.eof())
            response_body << "\n";
    }
    std::cout << std::endl << "REQUEST: FD " << fd << " langth "
        << request.length() << " byte:" << std::endl << request << std::endl;
    while (request.find("\n") < request.length())
        request.replace(request.find("\n"), 1, "<br>");
    response_body << request;

    std::stringstream response;
    response << "HTTP/1.1 200 OK\n"
             << "Host: localhost:8000\n"
             << "Content-Type: text/html; charset=UTF-8\n"
             << "Connection: close\n"
             << "Content-Length: " << response_body.str().length() <<"\n"
             << "\n"
             << response_body.str();

    send(fd, response.str().c_str(), response.str().length(), 0);

    request.clear();
    respondReady = false;

    close(fd);
    std::cout << "CLOSE " << fd << std::endl;

}

void Session::sendShortAnswer() {
    std::stringstream response;
    response << "HTTP/1.1 200 OK\n"
             << "Host: localhost:8000\n"
             << "Content-Type: text/html; charset=UTF-8\n"
             << "Connection: close\n"
             << "Content-Length: " << 0 <<"\n"
             << "\n";
    send(fd, response.str().c_str(), response.str().length(), 0);
}

bool Session::areRespondReady() {
    return respondReady;
}

Session::~Session() {

//    close(fd);
//    std::cout << "CLOSE " << fd << std::endl;
    std::cout << "DESTRUCTOR " << fd << std::endl;

}
