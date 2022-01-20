//
// Created by Magikarp Eunostus on 1/20/22.
//

#ifndef WEBSERV_LS_REGEX_HPP
#define WEBSERV_LS_REGEX_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <regex.h>

class ls_regex {
public:
	ls_regex();
	virtual ~ls_regex();

	void setfilter(const char* psz_filter);
	bool is_match(const char* psz_regex);

private:
	std::string m_str_filter;
	regex_t m_reg;
	bool m_is_valid_filter;
};

#endif //WEBSERV_LS_REGEX_HPP