//
// Created by Александр Шоршин on 31.10.2021.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <iostream>
//#include <string>
#include "Location.hpp"
#include <sstream>

//
//struct location{
//    std::vector<std::string> methods;
//    std::string index;
//    std::string root;
//    bool autoindex;
//    std::string exec;
//};

struct sErrorPage{
    unsigned int errorCode;
    std::string path;
};

class Config {

public:
    Config() : port(0), isReturn(false){};
    Config(const Config&);

    Config &operator = (const Config&);

    void setIP(const std::string &newIP);
    void setPort(int newPort);
    void setServerName(const std::string &param);
    void setErrorPage(const sErrorPage &param);
    void setReturnField(const std::string &param);
    void setReturnCode(const int &code);
    unsigned int getReturnCode() const;
    void addLocation(std::string locationName);
    Location &getLastLocation();
	std::vector<Location> getLocations();
    std::vector<Location>::iterator getLocation(int index);
	std::string getServerName() const;
    int getPort();
	std::string getIP() const;
	unsigned int getCode(int i) const;
	std::string getPath(int i) const;
	unsigned long getErrorPagesVectorSize() const;
	bool getIsReturn() const;
	void setIsReturn(bool);
	std::string getReturnField() const;

private:

	std::string ip;
    int port;
    std::string server_name;
    std::vector<sErrorPage> error_page;
    std::string returnField;
    int returnCode;
	bool isReturn;

    std::vector<Location> locations;

};


#endif
