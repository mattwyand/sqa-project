#include "util.h"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

std::string to_lower_case(const std::string &str)
{
	std::string result = str;
	for (char &c : result)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	return result;
}

std::string to_upper_case(const std::string &str)
{
	std::string result = str;
	for (char &c : result)
		c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
	return result;
}

std::string trim(const std::string &str)
{
	const char *whitespace = " \t\r\n";
	size_t first = str.find_first_not_of(whitespace);
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(whitespace);
	return str.substr(first, last - first + 1);
}

std::string pad_right(const std::string &str, int length)
{
	std::string result = str;
	while (static_cast<int>(result.size()) < length)
		result += ' ';
	return result;
}

bool is_number(const std::string &str)
{
	// An empty string can't be a number
	if (str.empty())
		return false;

	// strtod must consume the whole string, otherwise text like "12abc" would be accepted
	char *end = nullptr;
	double value = std::strtod(str.c_str(), &end);
	return end == str.c_str() + str.size() && std::isfinite(value);
}

long long to_cents(double dollars)
{
	return std::llround(dollars * 100.0);
}

std::string format_money(double dollars)
{
	char buffer[32];
	std::snprintf(buffer, sizeof(buffer), "%.2f", to_cents(dollars) / 100.0);
	return buffer;
}

std::string format_money_padded(double dollars, int width)
{
	char buffer[32];
	std::snprintf(buffer, sizeof(buffer), "%0*.2f", width, to_cents(dollars) / 100.0);
	return buffer;
}
