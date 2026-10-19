#pragma once
#include "game.h"

#include <string>
#include <vector>

// Represents a user along with their list of owned games, and possibly published games
class User
{
	friend class Program;

private:
	float wallet = 0.0f;
	std::string name = "";
	std::vector<std::string> owned_games{};

public:
	inline const float get_wallet_balance() const { return wallet; }
	inline const std::string get_name() const { return name; }
	inline const std::vector<std::string>& get_owned_games() const { return owned_games; }
};
