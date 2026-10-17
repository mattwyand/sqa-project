#include "game.h"

Game::Game(const std::string& name, const std::string& description, const std::string& seller_username, float price)
	: name(name), description(description), seller_username(seller_username), price(price) {}


// Temp game "database"
Game get_game(const std::string& name)
{
	if (name == "game1")
	{
		return Game(
			name,
			"This is game 1",
			"Nintendo",
			999.0f
		);
	}
	else if (name == "game2")
	{
		return Game(name,
			"This is game 2",
			"Bethesda",
			2.0f
		);
	}

	return Game(
		"DUMMY",
		"DUMMY_DESCRIPTION",
		"DUMMY_SELLER",
		0.0f);
}
