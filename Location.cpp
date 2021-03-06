//
// Created by Александр Шоршин on 13.12.2021.
//

#include "Location.hpp"

Location::Location(): autoindex(false), maxBody(false) {
}

Location::Location(std::string locationName): locationName(locationName),
autoindex(false), maxBody(false) {

}

Location::Location(const Location &other) {
    *this = other;
}

Location &Location::operator=(const Location &other) {

    this->locationName = other.locationName;
    this->index = other.index;
    this->root = other.root;
    this->methods = other.methods;
    this->autoindex = other.autoindex;
	this->exec = other.exec;
	this->max_body_size = other.max_body_size;
	this->upload_store = other.upload_store;
	this->maxBody = other.maxBody;

    return *this;
}

void Location::SetLocationName(std::string locationName) {

    this->locationName = locationName;

}

void Location::AddMethod(method method) {

    this->methods.push_back(method);

}

void Location::addIndex(std::string index) {

    this->index = index;

}

bool Location::operator<(const Location &oth) const {
	return this->locationName.size() < oth.locationName.size();
}

std::string Location::getLocationName() const{
	return locationName;
}

bool Location::isAutoIndex() const {
	return autoindex;
}

const std::string &Location::getIndex() const {
	return index;
}

bool Location::isMethodAvailable(const std::string &smethod) const {
	method searchingMethod;

    if (smethod == "GET")
        searchingMethod = GET;
    else if (smethod == "POST")
        searchingMethod = POST;
    else if (smethod == "DELETE")
        searchingMethod = DELETE;
	else if (smethod == "PUT")
		searchingMethod = PUT;

    for (size_t i = 0; i < methods.size(); ++i) {
        if (methods[i] == searchingMethod)
            return true;
    }
	return false;
}

void Location::setRoot(const std::string &param) {
    root = param;
}

void Location::setExec(const std::string &param) {
    exec = param;
}

void Location::setAutoindex(const std::string &param) {
    if (param == "on")
        autoindex = true;
    else
        autoindex = false;
}

void Location::setMaxBody_size(const std::string &param) {
    try {
		max_body_size = std::atoi(param.c_str());
    } catch (std::exception()){
        throw std::runtime_error("Wrong config file");
    }
}

void Location::setUploadStore(const std::string &param) {
    upload_store = param;
}

std::string Location::getRoot() const {
    return root;
}

std::string Location::getExec() const {
    return exec;
}

unsigned int Location::getMaxBodySize() const {
    return max_body_size;
}

std::string Location::getUploadStore() const {
    return upload_store;
}

bool Location::isMaxBody() const
{
	return maxBody;
}

void Location::setMaxBody(bool status)
{
	maxBody = status;
}

std::string Location::getAvailableMethods() const
{
	std::string result;
	for (size_t i = 0; i < this->methods.size(); ++i)
	{
		if (this->methods[i] == GET)
			result += "GET";
		else if (this->methods[i] == POST)
			result += "POST";
		else if (this->methods[i] == DELETE)
			result += "DELETE";
		else if (this->methods[i] == PUT)
			result += "PUT";
		if (this->methods.size() > 1 && this->methods.size() != i + 1)
			result += ", ";
	}
	return result;
}
