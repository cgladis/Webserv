//
// Created by Александр Шоршин on 29.10.2021.
//

#include <sys/socket.h>

int main()
{
	socket(AF_INET, SOCK_STREAM, 0);
    return 0;
}