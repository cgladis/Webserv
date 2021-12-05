//
// Created by Александр Шоршин on 05.12.2021.
//

#ifndef ALLCONFIGS_HPP
#define ALLCONFIGS_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include "Config.hpp"

class AllConfigs: public std::vector<Config>{

public:
    AllConfigs(const std::string& filename);

};


#endif //WEBSERV_ALLCONFIGS_HPP
