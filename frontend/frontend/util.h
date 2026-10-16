#pragma once
#include <string> 
#include <vector>

// Return a lowercase version of the string
std::string to_lower_case(const std::string &str);

// Split a string along a delimiter char
std::vector<std::string> split(const std::string &str, char delim);