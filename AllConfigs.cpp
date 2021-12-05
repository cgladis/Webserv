//
// Created by Александр Шоршин on 05.12.2021.
//

#include "AllConfigs.hpp"

AllConfigs::AllConfigs(const std::string &filename) {

    std::string line;

    int blockNumber = 0;
    std::ifstream fin(filename);

    while (std::getline(fin, line)) {

        if (fin.eof())
            return;
    }
    std::GetNextServer(&ifile);
}