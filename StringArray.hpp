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

    void addString(std::string newString);
    char **c_Arr();

private:
    size_t len_cArr();
    void delete_cArr();
    void refresh_cArr();
    std::vector<std::string> strArr;
    char **cArr;

};


#endif //WEBSERV_STRINGARRAY_HPP
