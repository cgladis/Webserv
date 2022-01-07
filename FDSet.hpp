//
// Created by Александр Шоршин on 04.01.2022.
//

#ifndef FDSET_HPP
#define FDSET_HPP

#include <iostream>
#include <vector>
#include <unistd.h>

class FDSet {
public:
    FDSet();
    fd_set &getWriteFDs();
    fd_set &getReadFDs();
    size_t lenWriteFds() const;
    size_t lenReadFds() const;
    void addWriteFD(int fd);
    void addReadFD(int fd);
    void addFD(int fd);
    void refresh();
    int getWriteFD(size_t index) const;
    int getReadFD(size_t index) const;
    void deleteFDfromReadFDs(int fd);
    void deleteFDfromWriteFDs(int fd);
    void deleteFD(int fd);
    bool isSetWriteFD(int fd) const;
    bool isSetReadFD(int fd) const;
    int getMaxFD() const;
    int select();
    void refreshMaxFD();

private:
    fd_set writeFds, readFds;
    fd_set writeFdsCopy, readFdsCopy;
    std::vector<int>arrayReadFds;
    std::vector<int>arrayWriteFds;
    int max_fd;
};

std::ostream  &operator << (std::ostream &out, const FDSet &other);


#endif
