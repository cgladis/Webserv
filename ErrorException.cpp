#include "ErrorException.hpp"

ErrorException::ErrorException() {}

ErrorException::~ErrorException() throw() {}

ErrorException::ErrorException(int code) {
	this->error_code = code;
}

