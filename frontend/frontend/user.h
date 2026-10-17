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
	std::vector<Game*> owned_games{};

public:
	//Game(const std::string& name, const std::vector<Game*> &owned_games);
	
	inline const std::string get_name() const { return name; }
	inline const std::vector<Game*>& get_owned_games() const { return owned_games; }
};
