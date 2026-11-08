#pragma once
#include <string>

// Represents one game that is available for sale, as read from the available games file.
class Game
{
	// 'Program' class should be able to edit private members
	friend class Program;

private:
	// The game's name (unique, at most 25 characters)
	std::string name = "";
	// Username of the account selling the game
	std::string seller_username = "";
	// Price in dollars (0.00 to 999.99)
	double price = 0.0;

public:
	Game() = default;

	// Returns the game's name
	inline const std::string &get_name() const { return name; }

	// Return the seller's username
	inline const std::string &get_seller_username() const { return seller_username; }

	// Return the game's price
	inline double get_price() const { return price; }
};
