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
                if (word == "index")
                    parseIndex(line, fileLine);
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
    word = nextWord(line);
    (this->end() - 1)->setPort(std::stoi(word));
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
}

void AllConfigs::parseErrorPage(std::string &line, int fileLine) {
    std::string word = nextWord(line);
    try {
        (this->end() - 1)->setErrorPage(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
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
}


