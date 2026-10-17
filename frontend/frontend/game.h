#pragma once
#include <string>

// Represents a game in the store / a user's library
class Game
{
	friend Game get_game(const std::string& name);

private:
	std::string name = "";
	std::string description = "";
	std::string seller_username = "";

	float price = 0.0f;

public:
	Game(const std::string& name, const std::string& description, const std::string& seller_username, float price);

	inline const std::string get_name() const { return name; }
	inline const std::string get_description() const { return description; }
	inline const std::string get_seller_username() const { return seller_username; }
	inline const float get_price() const { return price; }
};

Game get_game(const std::string& name);
