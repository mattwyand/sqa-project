#pragma once
#include <string>

// Represents a game in the store/user's library
class Game
{
	// 'Program' class should be able to edit private members
	friend class Program;

private:
	std::string name = "";
	std::string description = "";
	std::string seller_username = "";
	float price = 0.0f;

public:
	Game() = default;

	// Returns the game's name
	inline const std::string get_name() const { return name; }

	// Returns the game's description
	inline const std::string get_description() const { return description; }

	// Return the seller's username
	inline const std::string get_seller_username() const { return seller_username; }

	// Return the game's price
	inline const float get_price() const { return price; }
};
