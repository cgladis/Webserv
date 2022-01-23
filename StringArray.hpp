//
// Created by Александр Шоршин on 23.01.2022.
//

#ifndef WEBSERV_STRINGARRAY_HPP
#define WEBSERV_STRINGARRAY_HPP

#include <iostream>
#include <vector>

class StringArray {
public:
    StringArray();
    ~StringArray();

private:
    std::vector<std::string> strArr;
    char **cArr;

};


#endif //WEBSERV_STRINGARRAY_HPP
