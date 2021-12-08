//
// Created by Александр Шоршин on 31.10.2021.
//

#include "Config.hpp"

Config::Config(const Config &other) {
    *this = other;
}

Config &Config::operator=(const Config &other) {
    this->ip = other.ip;
    this->port = other.port;
    this->server_name = other.server_name;
    this->error_page = other.error_page;
    this->_return = other._return;
    return *this;
}

void Config::SetIP(const std::string &newIP) {
    ip = newIP;
}

void Config::SetPort(int newPort) {
    port = newPort;
}

void Config::AddLocation(const location &newLocation) {

    this->locations.push_back(newLocation);

}

IPAddress Config::GetIP() {
    return ip;
}

int Config::GetPort() {
    return port;
}


