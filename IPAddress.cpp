//
// Created by Александр Шоршин on 04.11.2021.
//

#include "IPAddress.hpp"

IPAddress::IPAddress(const std::string& ip): stringIP(ip) {

}

in_addr_t IPAddress::inet_addr() {
	return ::inet_addr(stringIP.c_str());
}