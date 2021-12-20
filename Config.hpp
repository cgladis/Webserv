//
// Created by Александр Шоршин on 31.10.2021.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <iostream>
//#include <string>
#include "IPAddress.hpp"
#include "Location.hpp"

//
//struct location{
//    std::vector<std::string> methods;
//    std::string index;
//    std::string root;
//    bool autoindex;
//    std::string exec;
//};

class Config {

public:
    Config(){}
    Config(const Config&);

    Config &operator = (const Config&);

    void setIP(const std::string &newIP);
    void setPort(int newPort);
    void setServerName(const std::string &param);
    void setErrorPage(const std::string &param);
    void setReturn(const std::string &param);
    void addLocation(std::string locationName);
    Location &getLastLocation();
	std::vector<Location> getLocations();

    IPAddress getIP();
    int getPort();

private:

    IPAddress ip;
    int port;
    std::string server_name;
    std::string error_page;
    std::string returnField;

    std::vector<Location> locations;

};


#endif
