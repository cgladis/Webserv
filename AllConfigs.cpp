//
// Created by Александр Шоршин on 05.12.2021.
//

#include "AllConfigs.hpp"

AllConfigs::AllConfigs(const std::string &filename):std::vector<Config>(),
        count(0) {

    Config newConfig;
    std::string word;

    std::string line;
    int level = 0;
    int fileLine = 0;

    std::ifstream fin(filename);

    while (std::getline(fin, line)) {
        fileLine++;
        while ((word = NextWord(line)) != ""){
            if (word == ";"){
                line.erase(0, word.length());
                continue;
            }
            if (word == "server" and level == 0) {
                this->push_back(newConfig);
                count++;
            }
            if (word == "{")
                level++;
            if (word == "}")
                level--;
            if (word == "listen" && level > 0) {
                line.erase(0, word.length());
                word = NextWord(line);
                this->end()->SetIP(word);
//                this[count - 1].SetIP(word);
                line.erase(0, word.length());
                word = NextWord(line);
                if (word != ":")
                    throw std::runtime_error("Wrong config file in line:");
                else
                    line.erase(0, word.length());
                word = NextWord(line);
//                this[count - 1].setPort(word);
                this->end()->SetPort(std::stoi(word));
            }
            line.erase(0, word.length());
            if (level < 0)
                throw std::runtime_error("Wrong config file in line:");
        }
        if (fin.eof())
            return;
    }
//    std::GetNextServer(&ifile);
}

std::string AllConfigs::NextWord(std::string) {
    return std::string();
}
