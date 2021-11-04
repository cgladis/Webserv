//
// Created by Александр Шоршин on 29.10.2021.
//

#include <iostream>
#include "Config.hpp"
#include "Socket.hpp"


int main(){

	Socket ServerSocket;
	ServerSocket.bind(80000);
    return 0;
}