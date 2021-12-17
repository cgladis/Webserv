//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Server.hpp"
#include "AllConfigs.hpp"

int main(){

//    try {
        AllConfigs configs("webserv.conf");
//    } catch (std::runtime_error &ex) {
//        std::cout << ex.what();
//        exit(0);
//    }

    Server webServer;

    try {
		webServer.addServers(configs);
        webServer.run(configs);
	}
    catch (std::runtime_error &ex) {
        std::cout << "ERROR - " << ex.what() << std::endl;
    }
    return 0;
}