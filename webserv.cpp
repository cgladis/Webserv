//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Server.hpp"
#include "AllConfigs.hpp"

int main(){

    AllConfigs configs("webserv.conf");
    Server webServer;

    try {
        for (int i = 0; i < configs.size(); ++i) {
            webServer.ADDServer(configs[i]);
        }
//            Server WebServer(8000, "127.0.0.1");
        webServer.init();
        webServer.run();
        }

//        while (1) {

//        }
    }
    catch (std::runtime_error &ex)
    {
        std::cout << "ERROR - " << ex.what() << std::endl;
    }
    return 0;
}