#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <sstream>

template <typename T>
std::string toString(T& in) {
	std::ostringstream ss;
	ss << in;
	return ss.str();
}

std::string Trim(const std::string& str, const std::string& whitespace = " \t") {
	const int strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const int strEnd = str.find_last_not_of(whitespace);
	const int strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

#endif