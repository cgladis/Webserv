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
        while ((word = NextWord(line)) != ""){
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
            } else {
                if (word == "listen")
                    parseListen(line, fileLine);
                if (word == "server_name")
                    parseServerName(line, fileLine);
                if (word == "return")
                    parseReturn(line, fileLine);
                if (word == "error_page")
                    parseErrorPage(line, fileLine);
                if (word == "location")
                    parseErrorPage(line, fileLine);
            }
        }
    }
    if (level != 0)
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
//    std::GetNextServer(&ifile);
}

std::string AllConfigs::NextWord(std::string &line) {

    size_t start_pos = line.find_first_not_of(" \t");
    line.erase(0, start_pos);
    std::string firstSimbol = line.substr(0, 1);
    if (firstSimbol.find_first_of("{};:") == 0) { //return 1 simbol
        std::string word = line.substr(0, 1);
        line.erase(0, word.length());
        return word;
    }
    size_t end_pos = line.find_first_of(" \t{};:");
    std::string word = line.substr(0, end_pos);
    line.erase(0, word.length());
    return word;
}

void AllConfigs::parseListen(std::string &line, int fileLine) {
    std::string word = NextWord(line);
    (this->end() - 1)->SetIP(word);
    word = NextWord(line);
    if (word != ":")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    word = NextWord(line);
    (this->end() - 1)->SetPort(std::stoi(word));
}

void AllConfigs::parseServerName(std::string &line, int fileLine) {
    std::string word = NextWord(line);
    try {
        (this->end() - 1)->SetServerName(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
}

void AllConfigs::parseErrorPage(std::string &line, int fileLine) {
    std::string word = NextWord(line);
    try {
        (this->end() - 1)->SetErrorPage(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }
}

void AllConfigs::parseReturn(std::string &line, int fileLine) {

    std::string word = NextWord(line);
    word += NextWord(line);
    word = NextWord(line);
    if (word != ":")
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    word += NextWord(line);

    try {
        (this->end() - 1)->SetReturn(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }

}

void AllConfigs::parseLocation(std::string &line, int fileLine) {

    std::string word = NextWord(line);
    try {
        (this->end() - 1)->AddLocation(word);
    } catch (std::exception &ex)
    {
        throw std::runtime_error("Wrong config file in line: "
                                 + std::to_string(fileLine));
    }

}


