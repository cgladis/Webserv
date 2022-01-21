//
// Created by Александр Шоршин on 05.12.2021.
//

#ifndef ALLCONFIGS_HPP
#define ALLCONFIGS_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "Socket.hpp"


class AllConfigs: public std::vector<Config>{

public:
    AllConfigs(const std::string& filename);
	void findIfSamePort();
	size_t getUniqIpPortVectorSize() const;
	std::string getIpPort(size_t i) const;
	Config getRightConfig(std::string, const Socket &) const;

private:
	int count;
	std::vector<std::string> uniqeIpPort;
	std::vector<Config> configsWithDefaultSerName;
    std::string nextWord(std::string&);
    void parseListen(std::string &line, int fileLine);
    void parseServerName(std::string &line, int fileLine);
    void parseErrorPage(std::string &line, int fileLine);
    void parseReturn(std::string &line, int fileLine);
    void parseLocation(std::string &line, int fileLine);
    void parseMethods(std::string &line, int fileLine);
    void parseIndex(std::string &line, int fileLine);
    void parseRoot(std::string &line, int fileLine);
    void parseExec(std::string &line, int fileLine);
    void parseAutoindex(std::string &line, int fileLine);
    void parseMaxBody(std::string &line, int fileLine);
    void parseUploadStore(std::string &line, int fileLine);

};


#endif
