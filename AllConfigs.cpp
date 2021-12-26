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

    std::string word = nextWord(line);
    word += nextWord(line);
    word = nextWord(line);
    if (word != ":")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    word += nextWord(line);

    try {
        (this->end() - 1)->setReturn(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
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
            else if (word == "PUT")
                (this->end() - 1)->getLastLocation().AddMethod(PUT);
            else if (word == "DELETE")
                (this->end() - 1)->getLastLocation().AddMethod(DELETE);
            else if (word == "," or word == ";")
                continue;
            else
                throw std::runtime_error("Wrong config file in line: "
                                         + std::to_string(fileLine));
        }
    } catch (std::exception &ex)
    {
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

void AllConfigs::checkServerNames() {
	Config conf1, conf2;
	for (size_t i = 0; i < this->size() - 1; ++i) {
		conf1 = this->operator[](i);
		for (size_t l = i + 1; l < this->size(); ++l) {
			conf2 = this->operator[](l);
			if (conf1.getIP() == conf2.getIP()
			&& conf1.getPort() == conf2.getPort()
			&& conf1.getServerName() == conf2.getServerName())
				throw std::runtime_error("The same Ip:Port:ServerName");
		}
	}
	// вектор с дефолтным server_name
	for (size_t i = 0; i < this->size(); ++i) {
		if (this->operator[](i).getServerName().empty())
			configsWithDefaultSerName.push_back(this->operator[](i));
	}

	// вектор уникальных ip:port
	std::string ipPort;
	for (size_t i = 0; i < this->size(); ++i) {
		conf1 = this->operator[](i);
		ipPort = conf1.getIP() + ":" + std::to_string(conf1.getPort());
		if (std::find(uniqeIpPort.begin(), uniqeIpPort.end(), ipPort) == uniqeIpPort.end())
			uniqeIpPort.push_back(ipPort);
	}

	// сравниваем их размеры
	if (uniqeIpPort.size() != configsWithDefaultSerName.size())
		throw std::runtime_error("err with Ip Port ServerName");
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
	std::vector<Config> sameIpPortConfs;
	std::string ip = socket.getIP();
	int port = socket.getPort();
	Config confSerName;
	Config confDefault;
	for (size_t i = 0; i < this->size(); ++i) {
		confSerName = this->operator[](i);
		if (confSerName.getPort() == port && confSerName.getIP() == ip && this->operator[](i).getServerName().empty()) {
			confDefault = this->operator[](i);
		}
		else if (confSerName.getPort() == port && confSerName.getIP() == ip && hostName == this->operator[](i)
		.getServerName()) {
			confSerName = this->operator[](i);
			return confSerName;
		}
	}
	return confDefault;
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
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->getLastLocation().setMaxBody(word);
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

