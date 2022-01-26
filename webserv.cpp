//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Server.hpp"
#include "AllConfigs.hpp"

int main(int ac, char **av, char **env){

	AllConfigs configs("webserv.conf");
	(void )av[ac];

	Server server;
	try {
		configs.findIfSamePort();
		server.addServers(configs);
		server.run(configs, env);
	}
	catch (std::runtime_error &ex) {
		std::cout << "ERROR - " << ex.what() << std::endl;
	}
	return (0);
}