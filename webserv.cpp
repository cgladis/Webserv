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
		webServer.addServers(configs);
        webServer.run();
	}
    catch (std::runtime_error &ex) {
        std::cout << "ERROR - " << ex.what() << std::endl;
    }
    return 0;
}