#include "util.h"

#include <sstream>

std::string to_lower_case(const std::string &str)
{
	std::string result = "";
	for (const char &c : str)
	{
		result += std::tolower(static_cast<unsigned char>(c));
	}
	return result;
}

std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> result; 
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delim))
	{
		result.push_back(token);
	}
	return result;
}
