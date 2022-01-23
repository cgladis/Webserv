//
// Created by Александр Шоршин on 23.01.2022.
//

#include "StringArray.hpp"

StringArray::StringArray() {

    cArr = new char* [1];
    cArr[0] = nullptr;

}

StringArray::~StringArray() {

    delete_cArr();

}

void StringArray::refresh_cArr() {

    delete_cArr();
    cArr = new char *[strArr.size() + 1];

    for (size_t i = 0; i <= strArr.size(); ++i) {
        if (i == strArr.size()){
            cArr[i] = nullptr;
            break;
        }
        cArr[i] = new char[strArr[i].size() + 1];
        std::strcpy(cArr[i], strArr[i].c_str());
    }
}

void StringArray::addString(std::string newString) {

    strArr.push_back(newString);

}

char **StringArray::c_Arr() {
    refresh_cArr();
    return cArr;
}

void StringArray::delete_cArr() {

    for (size_t i = 0; i <= len_cArr(); ++i) {
        delete [] cArr[i];
    }
    delete [] cArr;

}

size_t StringArray::len_cArr() {

    size_t count = 0;

    while (cArr[count] != nullptr){
        count++;
    }
    return count;
}

size_t StringArray::len() const{
    return strArr.size();
}

std::string &StringArray::operator[](size_t index) {
    return strArr[index];
}

std::ostream &operator << (std::ostream &out, StringArray &other){
    for (size_t i = 0; i < other.len(); ++i) {
        out << other[i] << " ";
    }
    return out;
}