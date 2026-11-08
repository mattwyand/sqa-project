#pragma once
#include <string>

// Small helper functions used throughout the Front End.

// Return a lowercase copy of the string
std::string to_lower_case(const std::string &str);

// Return an uppercase copy of the string
std::string to_upper_case(const std::string &str);

// Return the string without leading/trailing whitespace (spaces, tabs, \r, \n)
std::string trim(const std::string &str);

// Return 'str' padded on the right with spaces up to 'length' characters
// (used for the left-justified text fields in the data files)
std::string pad_right(const std::string &str, int length);

// Can the whole string be read as a number (e.g. "15", "15.00")?
bool is_number(const std::string &str);

// Convert a dollar amount to whole cents, rounding to the nearest cent.
// All money comparisons use cents so that e.g. 600.00 + 400.00 is exactly 1000.00.
long long to_cents(double dollars);

// Format a dollar amount with 2 decimals, e.g. 5 -> "5.00"
std::string format_money(double dollars);

// Format a dollar amount with 2 decimals, zero-padded to 'width' characters,
// e.g. format_money_padded(5, 9) -> "000005.00"
std::string format_money_padded(double dollars, int width);
