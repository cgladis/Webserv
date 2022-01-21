//
// Created by Александр Шоршин on 05.12.2021.
//

#include "AllConfigs.hpp"

AllConfigs::AllConfigs(const std::string &filename):std::vector<Config>(),
        count(0) {

	//    Config newConfig;
    std::string word;

    std::string line;
    int level = 0;
    int fileLine = 0;

    std::ifstream fin(filename);

    while (std::getline(fin, line)) {
        fileLine++;
        while ((word = nextWord(line)) != ""){
            if (word == ";") {
                continue;
            }
            if (word == "{") {
                level++;
                continue;
            }
            if (word == "}"){
                level--;
                continue;
            }
            if (level < 0)
                throw std::runtime_error("Wrong config file in line: "
                                         + std::to_string(fileLine));
            if (level == 0){
                if (word == "server") {
                    this->push_back(Config());
                    count++;
                } else
                    throw std::runtime_error("Wrong config file in line: "
                                             + std::to_string(fileLine));
            } else if  (level == 1) {
                if (word == "listen")
                    parseListen(line, fileLine);
                else if (word == "server_name")
                    parseServerName(line, fileLine);
                else if (word == "return")
                    parseReturn(line, fileLine);
                else if (word == "error_page")
                    parseErrorPage(line, fileLine);
                else if (word == "location")
                    parseLocation(line, fileLine);
                else
                    throw std::runtime_error("Wrong config file in line: "
                                             + std::to_string(fileLine));
            } else if (level == 2){
                if (word == "methods")
                    parseMethods(line, fileLine);
                else if (word == "index")
                    parseIndex(line, fileLine);
                else if (word == "root")
                    parseRoot(line, fileLine);
                else if (word == "exec")
                    parseExec(line, fileLine);
                else if (word == "autoindex")
                    parseAutoindex(line, fileLine);
                else if (word == "max_body")
                    parseMaxBody(line, fileLine);
                else if (word == "upload_store")
                    parseUploadStore(line, fileLine);
                else
                    throw std::runtime_error("Wrong config file in line: "
                                             + std::to_string(fileLine));
            } else
                throw std::runtime_error("Wrong config file in line: "
                                         + std::to_string(fileLine));
        }
    }
    if (level != 0)
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
//    std::GetNextServer(&ifile);
}

std::string AllConfigs::nextWord(std::string &line) {

    size_t start_pos = line.find_first_not_of(" \t");
    line.erase(0, start_pos);
    std::string firstSimbol = line.substr(0, 1);
    if (firstSimbol.find_first_of("{};:,") == 0) { //return 1 simbol
        std::string word = line.substr(0, 1);
        line.erase(0, word.length());
        return word;
    }
    size_t end_pos = line.find_first_of(" \t{};:,"); //stop simbol
    std::string word = line.substr(0, end_pos);
    line.erase(0, word.length());
    return word;
}

void AllConfigs::parseListen(std::string &line, int fileLine) {
    std::string word = nextWord(line);
	(this->end() - 1)->setIP(word);
    word = nextWord(line);
    if (word != ":")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    try {
        word = nextWord(line);
        (this->end() - 1)->setPort(std::stoi(word));
    }catch (std::exception &ex){
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseServerName(std::string &line, int fileLine) {
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->setServerName(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseErrorPage(std::string &line, int fileLine) {
    std::string word;
    sErrorPage errorPage;

    try {
        word = nextWord(line);
        errorPage.errorCode = std::atoi(word.c_str());
        word = nextWord(line);
        errorPage.path = word;
        (this->end() - 1)->setErrorPage(errorPage);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseReturn(std::string &line, int fileLine) {
	(this->end() - 1)->setIsReturn(true);

	std::string word = nextWord(line);
	unsigned int code;
	std::stringstream ss;
	ss << word;
	ss >> code;
	(this->end() - 1)->setReturnCode(code);

	std::size_t found = line.find_first_of(":");
	if (found != std::string::npos)
	{
		word = nextWord(line);
		word += nextWord(line);
		word += nextWord(line);
	}
	else
		word = nextWord(line);
	(this->end() - 1)->setReturnField(word);

	if ((word = nextWord(line)) != ";")
		throw std::runtime_error("Wrong config file in line: "
								 + std::to_string(fileLine));
}

void AllConfigs::parseLocation(std::string &line, int fileLine) {

    std::string word = nextWord(line);
    try {
        (this->end() - 1)->addLocation(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
}

void AllConfigs::parseMethods(std::string &line, int fileLine) {
    std::string word;
    try {
        while (word != ";"){
            word = nextWord(line);
            if (word == "GET")
                (this->end() - 1)->getLastLocation().AddMethod(GET);
            else if (word == "POST")
                (this->end() - 1)->getLastLocation().AddMethod(POST);
            else if (word == "DELETE")
                (this->end() - 1)->getLastLocation().AddMethod(DELETE);
			else if (word == "PUT")
				(this->end() - 1)->getLastLocation().AddMethod(PUT);
            else if (word == "," or word == ";")
                continue;
            else
                throw std::runtime_error("Wrong config file in line: "
                                         + std::to_string(fileLine));
        }
    } catch (std::exception &ex) {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
}

void AllConfigs::parseIndex(std::string &line, int fileLine) {

    std::string word = nextWord(line);
    try {
        (this->end() - 1)->getLastLocation().addIndex(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::findIfSamePort() {
	Config conf1;
	std::string ipPort;
	for (size_t i = 0; i < this->size(); ++i) {
		conf1 = this->operator[](i);
		ipPort = conf1.getIP() + ":" + std::to_string(conf1.getPort());
		if (std::find(uniqeIpPort.begin(), uniqeIpPort.end(), ipPort) == uniqeIpPort.end())
			uniqeIpPort.push_back(ipPort);
		else
			throw std::runtime_error("same ports(");
	}
}

size_t AllConfigs::getUniqIpPortVectorSize() const {
	return uniqeIpPort.size();
}

std::string AllConfigs::getIpPort(size_t i) const {
	if (i > uniqeIpPort.size())
		throw std::runtime_error("too big value");
	return uniqeIpPort[i];
}

Config AllConfigs::getRightConfig(std::string hostName, const Socket &socket) const {
	hostName = hostName.substr(0, hostName.find(':'));
	std::string ip = socket.getIP();
	int port = socket.getPort();
	Config defaultServ;
	Config currentServ;
	bool defServFound = false;

	for (size_t i = 0; i < this->size(); ++i) {
		currentServ = this->operator[](i);
		if (currentServ.getPort() == port && currentServ.getIP() == ip && !defServFound) {
			defaultServ = currentServ;
			defServFound = true;
		}
		else if (currentServ.getPort() == port && currentServ.getIP() == ip && hostName == this->operator[](i)
		.getServerName()) {
			return currentServ;
		}
	}
	return defaultServ;
}

void AllConfigs::parseRoot(std::string &line, int fileLine) {
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->getLastLocation().setRoot(word);
    } catch (std::exception &ex) {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseExec(std::string &line, int fileLine) {
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->getLastLocation().setExec(word);
    } catch (std::exception &ex) {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseAutoindex(std::string &line, int fileLine) {
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->getLastLocation().setAutoindex(word);
    } catch (std::exception &ex) {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseMaxBody(std::string &line, int fileLine) {
	(this->end() - 1)->getLastLocation().setMaxBody(true);
    std::string word = nextWord(line);
    try {
		(this->end() - 1)->getLastLocation().setMaxBody_size(word);
    } catch (std::exception &ex) {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

void AllConfigs::parseUploadStore(std::string &line, int fileLine) {
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->getLastLocation().setUploadStore(word);
    } catch (std::exception &ex) {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
    if ((word = nextWord(line)) != ";")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
}

