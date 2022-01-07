//
// Created by Александр Шоршин on 04.01.2022.
//

#include "FDSet.hpp"

FDSet::FDSet():max_fd(0) {

    FD_ZERO(&writeFds);
    FD_ZERO(&readFds);

}

void FDSet::addWriteFD(int fd) {
    FD_SET(fd, &writeFds);
    arrayWriteFds.push_back(fd);

    if (fd > max_fd) max_fd = fd;
}

void FDSet::addReadFD(int fd) {

    FD_SET(fd, &readFds);
    arrayReadFds.push_back(fd);

    if (fd > max_fd) max_fd = fd;

}

void FDSet::addFD(int fd) {
    //add writeFD and readFD
    FD_SET(fd, &readFds);
    FD_SET(fd, &writeFds);
    arrayWriteFds.push_back(fd);
    arrayReadFds.push_back(fd);

    if (fd > max_fd) max_fd = fd;
    
}


fd_set &FDSet::getWriteFDs() {
    FD_COPY(&writeFds, &writeFdsCopy);
    return writeFdsCopy;
}

fd_set &FDSet::getReadFDs() {
    FD_COPY(&readFds, &readFdsCopy);
    return readFdsCopy;
}

size_t FDSet::lenWriteFds() const {
    return arrayWriteFds.size();
}

size_t FDSet::lenReadFds() const {
    return arrayReadFds.size();
}

void FDSet::refresh() {

    FD_ZERO(&writeFds);
    FD_ZERO(&readFds);

    for (size_t i = 0; i < lenWriteFds(); ++i) {
        FD_SET(arrayWriteFds[i], &writeFds);
    }

    for (size_t i = 0; i < lenReadFds(); ++i) {
        FD_SET(arrayReadFds[i], &readFds);
    }

    refreshMaxFD();
}

int FDSet::getWriteFD(size_t index) const {
    return arrayWriteFds[index];
}

int FDSet::getReadFD(size_t index) const {
    return arrayReadFds[index];
}

void FDSet::deleteFDfromReadFDs(int fd) {
    FD_CLR(fd, &readFds);
    arrayReadFds.erase(
            std::remove(arrayReadFds.begin(), arrayReadFds.end(),
                        fd));

    if (fd == max_fd)
        refreshMaxFD();

}

void FDSet::deleteFDfromWriteFDs(int fd) {
    FD_CLR(fd, &writeFds);
    arrayWriteFds.erase(
            std::remove(arrayWriteFds.begin(), arrayWriteFds.end(),
                        fd));

    if (fd == max_fd)
        refreshMaxFD();
}

//delete fd from readFds and writeFds
void FDSet::deleteFD(int fd) {
    FD_CLR(fd, &readFds);
    FD_CLR(fd, &writeFds);
    arrayReadFds.erase(
            std::remove(arrayReadFds.begin(), arrayReadFds.end(),
                        fd), arrayReadFds.end());
    arrayWriteFds.erase(
            std::remove(arrayWriteFds.begin(), arrayWriteFds.end(),
                        fd), arrayWriteFds.end());

    if (fd == max_fd)
        refreshMaxFD();

}

bool FDSet::isSetWriteFD(int fd) const{
    return FD_ISSET(fd, &writeFds);
}

bool FDSet::isSetReadFD(int fd) const{
    return FD_ISSET(fd, &readFds);
}

int FDSet::select() {
    refresh();
    usleep(1000);
    int result = ::select(max_fd + 1, &readFds, &writeFds, nullptr, nullptr);
    return result;
}

int FDSet::getMaxFD() const {
    return max_fd;
}

void FDSet::refreshMaxFD() {
    int maxReadFD;
    int maxWriteFD;
    if (arrayReadFds.size() > 0)
        maxReadFD = *max_element(arrayReadFds.begin(), arrayReadFds.end());
    else
        maxReadFD = 0;
    if (arrayWriteFds.size() > 0)
        maxWriteFD = *max_element(arrayWriteFds.begin(), arrayWriteFds.end());
    else
        maxWriteFD = 0;
    max_fd = std::max(maxReadFD, maxWriteFD);

}


std::ostream  &operator << (std::ostream &out, const FDSet &other){
    out << "read_fd(";
    for (size_t i = 0; i < other.lenReadFds(); ++i) {
        out << other.getReadFD(i);
        if (i == other.lenReadFds() - 1) continue;
        out << ", ";
    }
    out << "), write_fd(";
    for (size_t i = 0; i < other.lenWriteFds(); ++i) {
        out << other.getWriteFD(i);
        if (i == other.lenWriteFds() - 1) continue;
        out << ", ";
    }
    out << "); after select: read_fd(";
    int count = 0;
    for (int i = 0; i < 1000; ++i) {
        if (other.isSetReadFD(i)) {
            if (count != 0) out << ", ";
            out << i;
            count++;
        }
    }
    out << "), write_fd(";
    count = 0;
    for (int i = 0; i < 1000; ++i) {
        if (other.isSetWriteFD(i)) {
            if (count != 0) out << ", ";
            out << i;
            count++;
        }
    }
    out << "); max_fd: " << other.getMaxFD();
    return out;
}

