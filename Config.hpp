//
// Created by Александр Шоршин on 31.10.2021.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <iostream>
//#include <string>
#include "IPAddress.hpp"


struct location{
    std::vector<std::string> methods;
    std::string index;
    std::string root;
    bool autoindex;
    std::string exec;
};

class Config {

public:
    Config(){}
    Config(const Config&);

    Config &operator = (const Config&);

    void SetIP(const std::string &newIP);
    void SetPort(int newPort);
    void AddLocation(const location &newLocation);

    IPAddress GetIP();
    int GetPort();

private:

    IPAddress ip;
    int port;
    std::string server_name;
    std::string error_page;
    std::string _return;

    std::vector<location> locations;

};


#endif
