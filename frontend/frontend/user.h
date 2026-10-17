#pragma once
#include "game.h"

#include <string>
#include <vector>

// Represents a user along with their list of owned games, and possibly published games
class User
{
	friend User get_user(const std::string& username);

private:
	std::string name = "";
	std::vector<Game> owned_games{};

public:
	inline const std::string get_name() const { return name; }

	// TODO: Make this take pointers to Games?
	inline const std::vector<Game>& get_owned_games() const { return owned_games; }
};

// TEMP: Get a user from the database of users
User get_user(const std::string &username);