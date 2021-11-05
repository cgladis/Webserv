//
// Created by Александр Шоршин on 04.11.2021.
//

#include "IPAddress.hpp"

IPAddress::IPAddress(const std::string& ip) {

	stringIP = ip;
	this->checkIP();

}

in_addr_t IPAddress::inet_addr() {
	return ::inet_addr(stringIP.c_str());
}

void IPAddress::checkIP() {

//    std::string tmp;
//
//    const char* pattern =
//            "\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
//            "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
//            "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
//            "\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
//    regex_match(stringIP, pattern);
//
    if (stringIP.find_first_not_of("0123456789.") < stringIP.length())
		throw std::runtime_error("Wrong IP");
//    std::regex
//    Regex^ emailregex = gcnew Regex("(?<user>[^@]+)@(?<host>.+)");
//
//    }

// Написать нормальную проверку

}