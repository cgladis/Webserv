//
// Created by Александр Шоршин on 13.12.2021.
//

#include "Location.hpp"

Location::Location(): autoindex(true) {
}

Location::Location(std::string locationName): locationName(locationName), autoindex(true) {

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

bool Location::isMethodAvailable(const std::string &smethod) {
	method searchingMethod;

    if (smethod == "GET")
        searchingMethod = GET;
    else if (smethod == "POST")
        searchingMethod = POST;
    else if (smethod == "DELETE")
        searchingMethod = DELETE;

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
    else if (param == "off")
        autoindex = false;
    else
        throw std::runtime_error("Wrong config file");
}

void Location::setMaxBody(const std::string &param) {
    try {
        max_body = std::atoi(param.c_str());
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

unsigned int Location::getMaxBody() const {
    return max_body;
}

std::string Location::getUploadStore() const {
    return upload_store;
}


