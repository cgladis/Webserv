//
// Created by Александр Шоршин on 13.12.2021.
//

#include "Location.hpp"

Location::Location(): autoindex(false) {

}

Location::Location(std::string locationName): locationName(locationName), autoindex(false) {

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

    return *this;
}

void Location::SetLocationName(std::string locationName) {

    this->locationName = locationName;

}

void Location::AddMethod(method method) {

    this->methods.push_back(method);

}
