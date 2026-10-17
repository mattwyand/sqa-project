#include "user.h"
#include "game.h"

// Temp user "database"
User get_user(const std::string& username)
{
	User u;

	if (username == "admin")
	{
		u.name = "admin";
		u.owned_games = {
			get_game("game1"),
			get_game("game2"),
		};
	}
	else if (username == "carpet")
	{
		u.name = "admin";
		u.owned_games = {
			get_game("game1"),
			get_game("game2"),
		};
	}

	return u;
}
