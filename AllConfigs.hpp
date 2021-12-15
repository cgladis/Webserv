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

private:
    int count;
    std::string NextWord(std::string&);

    void parseListen(std::string &line, int fileLine);
    void parseServerName(std::string &line, int fileLine);
    void parseErrorPage(std::string &line, int fileLine);
    void parseReturn(std::string &line, int fileLine);
    void parseLocation(std::string &line, int fileLine);

};


#endif
