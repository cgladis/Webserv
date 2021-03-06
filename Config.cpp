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
    this->isReturn = other.isReturn;
	this->returnCode = other.returnCode;
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

void Config::setErrorPage(const sErrorPage &param) {
    error_page.push_back(param);
}

void Config::setReturnField(const std::string &param) {
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

unsigned int Config::getCode(int i) const {
	return error_page[i].errorCode;
}

std::string Config::getPath(int i) const {
	return error_page[i].path;
}

unsigned long Config::getErrorPagesVectorSize() const {
	return error_page.size();
}

bool Config::getIsReturn() const
{
	return isReturn;
}

void Config::setIsReturn(bool status)
{
	isReturn = status;
}
void Config::setReturnCode(const int &code)
{
	returnCode = code;
}

unsigned int Config::getReturnCode() const
{
	return (returnCode);
}

std::string Config::getReturnField() const {
	return returnField;
}
