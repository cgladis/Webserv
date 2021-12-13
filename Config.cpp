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
    this->returnField = other.returnField;
    this->locations = other.locations;
    return *this;
}

void Config::SetIP(const std::string &newIP) {
    ip = newIP;
}

void Config::SetPort(int newPort) {
    port = newPort;
}

void Config::AddLocation(std::string locationName) {

    this->locations.push_back(Location(locationName));

}

IPAddress Config::GetIP() {
    return ip;
}

int Config::GetPort() {
    return port;
}

void Config::SetServerName(const std::string &param) {
    server_name = param;
}

void Config::SetErrorPage(const std::string &param) {
    error_page = param;

}

void Config::SetReturn(const std::string &param) {

    returnField = param;

}
