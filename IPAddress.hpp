//
// Created by Александр Шоршин on 04.11.2021.
//

#ifndef IPADDRESS_HPP
#define IPADDRESS_HPP

#include <iostream>
#include <string>
#include <regex>
#include <arpa/inet.h>

class IPAddress
{
public:
    IPAddress();
	IPAddress(const std::string& ip);

    void setIP(const std::string& ip);
    std::string getIP() const;

	in_addr_t inet_addr();

private:
	std::string stringIP;
	void checkIP();

};

std::ostream  &operator << (std::ostream &out, const IPAddress &other);


#endif
