//
// Created by Александр Шоршин on 31.10.2021.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <iostream>
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

private:

    std::vector<location> locations;
    IPAddress listen;
    int port;
    std::string server_name;
    std::string error_page;
    std::string _return;

};


#endif
