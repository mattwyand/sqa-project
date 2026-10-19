#include "util.h"

#include <sstream>
#include <charconv> // For converteding std::string to float

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

bool is_float(const std::string &str)
{
	if (str.empty())
		return false;
	
	float value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return ec == std::errc() && ptr == str.data() + str.size();
}
