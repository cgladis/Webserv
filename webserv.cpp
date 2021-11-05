//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Server.hpp"

int main(){

    try {
        Server WebServer(8000, "127.0.0.1");
        WebServer.run();
        WebServer.processingRequests();
    }
    catch (std::runtime_error &ex)
    {
        std::cout << "ERROR - " << ex.what() << std::endl;
    }
    return 0;
}