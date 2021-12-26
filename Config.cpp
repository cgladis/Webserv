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

void Config::setIP(const std::string &newIP) {
    ip = newIP;
}

void Config::setPort(int newPort) {
    port = newPort;
}

void Config::addLocation(std::string locationName) {
    this->locations.push_back(Location(locationName));
}

int Config::getPort() {
    return port;
}

void Config::setServerName(const std::string &param) {
    server_name = param;
}

void Config::setErrorPage(const std::string &param) {
    error_page = param;
}

void Config::setReturn(const std::string &param) {
    returnField = param;
}

Location &Config::getLastLocation() {
    return *(locations.end() - 1);
}

std::vector<Location> Config::getLocations() {
	std::sort(locations.begin(), locations.end());
	std::reverse(locations.begin(), locations.end());
	return locations;
}

std::string Config::getServerName() const {
	return server_name;
}

std::string Config::getIP() const {
	return ip;
}

std::vector<Location>::iterator Config::getLocation(int index) {
    return locations.begin() + index;
}
