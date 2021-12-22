//
// Created by Александр Шоршин on 13.12.2021.
//

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <iostream>

enum method{
	GET, POST, PUT, DELETE
};

class Location {
public:
    Location();
    Location(std::string locationName);
    Location(const Location&);
	bool operator<(const Location& oth) const;
    Location &operator = (const Location&);

    void SetLocationName(std::string locationName);
	std::string getLocationName() const;
    void AddMethod(method method);
    void addIndex(std::string index);
	bool isAutoIndex() const;
	const std::string &getIndex() const;
	bool isMethodAvailable(const std::string &);

private:
    std::string locationName;
    std::vector<method> methods;
    std::string index;
    std::string root;
    bool autoindex;
    std::string exec;
};


#endif
