//
// Created by Magikarp Eunostus on 1/20/22.
//

#include "ls_regex.hpp"

ls_regex::ls_regex()
{
	m_is_valid_filter = false;
	m_str_filter = "";
}

ls_regex::~ls_regex()
{
}

void ls_regex::setfilter(const char* psz_filter)
{
	int i_rc = 0;

	m_str_filter = (NULL != psz_filter) ? psz_filter : "";
	i_rc = regcomp(&m_reg, m_str_filter.c_str(), REG_NOSUB | REG_EXTENDED);
	m_is_valid_filter = (0 == i_rc);
}

bool ls_regex::is_match(const char* psz_regex)
{
	int i_rc = 0;

	if (NULL == psz_regex) {
		return false; // not match
	}

	if (!m_is_valid_filter) {
		return true; // if filter is invalid or not set, return as match
	}

	i_rc = regexec(&m_reg, psz_regex, 0, NULL, 0);
	return (REG_NOMATCH != i_rc);
}
