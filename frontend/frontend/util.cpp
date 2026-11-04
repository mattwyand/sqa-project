#include "util.h"

#include <sstream>
#include <iomanip> // for std::fixed and std::setprecision
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
	// We read the string through a stringstream so we can use std::getline
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
	// An empty string can't be converted to a float
	if (str.empty())
		return false;
	
	// Try to convert string to a float.
	// If successful, return true. If not successful, return false.
	float value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return ec == std::errc() && ptr == str.data() + str.size();
}

std::string float_to_fmtstring(float f)
{
	// Use an ostringstream to truncate the precision of the float to 2 decimal points
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(2) << f;
	return ss.str();
}
