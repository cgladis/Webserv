//
// Created by Александр Шоршин on 05.11.2021.
//

#ifndef SESSION_HPP
#define SESSION_HPP

#include <sys/socket.h>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <string>
#include "AllConfigs.hpp"
#include "Socket.hpp"
#include "ErrorException.hpp"
#include <map>
#include <sys/stat.h>
#include <cstdio>
#include <string>
#include <csignal>
#include "StringArray.hpp"
#define C_WHITE "\033[0m"
#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_BLUE "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_MINT "\033[36m"


#define READING_BUFF 100

class Session {
public:
    Session(int fd, const Socket &);
	Session &operator=(const Session& oth);
    ~Session();

	void makeAndSendResponse(int fd, const std::string& response_body, unsigned int code = 200, const std::string
	&status = "OK", std::string response_header = "", bool nobody= false);
	void parseAsChunked();
	void parseHeader();
    void getRequest(const AllConfigs &configs);
	Location getMyLocation(const std::vector<Location> &locations, const std::string &url);
	void sendAnswer(char **env);
	void handleAsDir();
	void errorPageHandle(unsigned int);
    int get_fd() const;
    bool areRespondReady() const;
	void handlePostRequest(char **);
	void handlePutRequest();
	void handleGetRequest(char **);
	void handleAsCGI(char **env);
	void handleDeleteRequest();
	std::string openAndReadTheFile(const std::string &);
	void initializeAndCheckData();
    void read_cgi();
    int time_passed() const;
    bool are_need_to_read_cgi() const;
    bool is_answer_sent() const;
    bool no_request() const;
    bool is_request_received() const;

private:
    int fd;
    pid_t pid;
    bool respondReady;
    bool need_to_read_cgi;
    std::string request;

	std::string path;
	Config config;
	Location location;
	Socket sesSocket;
	std::map<std::string, std::string> header;
	std::map<std::string, std::string> argsForCgi;
	std::map<std::string, std::string> mimeTypes;
    std::string argsForCgiString;
	std::string uploadedFilename;
	std::string fileText;
	bool isChunked;
	int contentLength;
	bool isHeaderRead;
	bool isSGI;
	long bodySum;
    int cgi_fd_in[2];
    int cgi_fd_out[2];
    clock_t start_time;
    bool answer_sent;
    bool request_received;

};


#endif
