//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Server.hpp"

int main(){

	Server WebServer(8000, "127.0.0.1");
    WebServer.run();
    WebServer.processingRequests();

    return 0;
}