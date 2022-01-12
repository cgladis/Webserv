#ifndef ERROREXCEPTION_HPP
#define ERROREXCEPTION_HPP
#include <string>

class ErrorException : public std::exception {
public:
	ErrorException();
	ErrorException(int code);
	~ErrorException() throw();
	int error_code;

private:

};


#endif
