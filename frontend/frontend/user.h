#pragma once
#include "game.h"

#include <string>
#include <vector>

// Represents a user along with their list of owned games, and possibly published games
class User
{
	friend class Program;

private:
	std::string name = "";
	std::vector<std::string> owned_games{};

public:
	inline const std::string get_name() const { return name; }
	inline const std::vector<std::string>& get_owned_games() const { return owned_games; }
};
