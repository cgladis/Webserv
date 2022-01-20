//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Server.hpp"
#include "AllConfigs.hpp"

int main(){

	AllConfigs configs("webserv.conf");

	Server server;
	try {
		configs.findIfSamePort();
		server.addServers(configs);
		server.run(configs);
	}
	catch (std::runtime_error &ex) {
		std::cout << "ERROR - " << ex.what() << std::endl;
	}
}