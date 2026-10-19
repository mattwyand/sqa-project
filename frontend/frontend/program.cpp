#include "program.h"

#include "commands.h"
#include "util.h"
#include "game.h"

#include <iostream>
#include <vector>

#include "thirdparty/json.hpp"
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace
{
	const std::string DATABASE_FILENAME = "database.json";
	const std::string ALL_GAMES = "__all__";
}

void Program::run()
{
	init_data_path();
	read_database();
	
	print_startup();

	while (!exit_program)
	{
		build_command_list();
		print_main_menu();

		// Get input
		std::string input;
		std::getline(std::cin, input);

		// TODO: strip input of whitespace

		// Transform to lowercase so input can be any case
		input = to_lower_case(input);

		// See if we input one of the commands listed
		auto args = split(input, ' ');
		for (Command* command : commands)
		{
			if (args[0] == command->get_name())
			{
				command->run(args);
				break;
			}
		}

		std::cout << "\n\n";
	}
}

void Program::print_startup()
{
	std::cout << " _   _       _       ____  _                       \n"
		<< "| \\ | | ___ | |_    / ___|| |_ ___  __ _ _ __ ___\n"
		<< "|  \\| |/ _ \\| __|___\\___ \\| __/ _ \\/ _` | '_ ` _ \\ \n"
		<< "| |\\  | (_) | ||_____|__) | ||  __/ (_| | | | | | |\n"
		<< "|_| \\_|\\___/ \\__|   |____/ \\__\\___|\\__,_|_| |_| |_|\n";
	std::cout << "Version 0.0.1\n\n\n";
}

void Program::print_main_menu()
{
	if (logged_in)
	{
		// Print username in menu header if we're logged in
		std::cout << "-- Menu (" << user->get_name() << ") --\n";
	}
	else
	{
		std::cout << "-- Menu --\n";
	}

	for (Command* command : commands)
	{
		command->print_description();
	}

	std::cout << "\n>";
}

User *Program::get_user(const std::string &name) const
{
	if (user_database.count(name))
	{
		return user_database.at(name).get();
	}
	else
	{
		return nullptr;
	}
}

Game *Program::get_game(const std::string& name) const
{
	if (game_database.count(name))
	{
		return game_database.at(name).get();
	}
	else
	{
		return nullptr;
	}
}

const std::filesystem::path &Program::get_data_path() const
{
	LB_ASSERT(data_path.string().size() > 0, "Data path has not been initialized.");
	return data_path;
}

void Program::list_owned_games()
{
	std::cout << user->get_name() << "'s games:\n";

	for (auto& game_name : user->get_owned_games())
	{
		// List all games if we encounter the special ALL_GAMES tag
		if (game_name == ALL_GAMES)
		{
			for (auto &pair : game_database)
			{
				Game *g = pair.second.get();
				std::cout << "\"" << g->get_name() << "\" - " << g->get_description() << "\n"
					<< "Seller: " << g->get_seller_username() << "\n"
					<< "Price: $" << g->get_price() << "\n\n";
			}
			continue;
		}

		Game *game = get_game(game_name);
		if (!game)
			continue;

		std::cout << "\"" << game->get_name() << "\" - " << game->get_description() << "\n"
			<< "Seller: " << game->get_seller_username() << "\n"
			<< "Price: $" << game->get_price() << "\n\n";
	}
}

void Program::login(const std::string& name)
{
	user = get_user(name);
	if (!user)
	{
		std::cout << "User '" << name << "' does not exist.\n";
		return;
	}

	logged_in = true;
	std::cout << "Logged in as '" << name << "'\n";

	// Check if we're an admin
	if (user->get_name() == "admin")
	{
		std::cout << "Began Admin session. Privileged/unprivileged transactions are available.\n";
	}
}

void Program::logout()
{
	logged_in = false;
	user = nullptr;
}

void Program::exit()
{
	exit_program = true;
	std::cout << "Closing program...\n";
}

void Program::init_data_path()
{
	fs::path base_path = fs::current_path();
	fs::path current_path;

	// Start in our current path. Keep looking up until we find the data dir.
	fs::path temp = base_path;
	do
	{
		LB_ASSERT(temp.string().size() > 0, "Could not find data folder.");

		current_path = temp / "data";
		temp = temp.parent_path();
	} while (!fs::is_directory(current_path));

	// Append a slash to the end (weird syntax)
	current_path /= "";

	// We've found our data path
	data_path = current_path;
}

void Program::read_database()
{
	game_database.clear();
	user_database.clear();

	// Read json data
	std::ifstream f(data_path / DATABASE_FILENAME);
	if (!f.is_open())
	{
		// TODO: Create new database file
		std::string error = "'" + data_path.string() + DATABASE_FILENAME + "' does not exist. Database cannot be populated.";
		LB_ASSERT(false, error.c_str());
		return;
	}

	json data = json::parse(f);
	f.close();

	// -- Games --
	LB_ASSERT(data["games"].is_array(), "'games' list does not exist in database.");
	for (auto &game : data["games"])
	{
		// Verify fields
		LB_ASSERT(game["name"].is_string(),			"Game 'name' does not exist or is wrong type.");
		LB_ASSERT(game["description"].is_string(),	"Game 'description' does not exist or is wrong type.");
		LB_ASSERT(game["seller"].is_string(),		"Game 'seller' does not exist or is wrong type.");
		LB_ASSERT(game["price"].is_number(),		"Game 'price' does not exist or is wrong type.");

		auto g = std::make_unique<Game>();
		g->name = game["name"];
		g->description = game["description"];
		g->seller_username = game["seller"];
		g->price = game["price"];

		game_database.insert({
			g->name,
			std::move(g)
		});
	}

	// -- Users --
	LB_ASSERT(data["users"].is_array(), "'users' list does not exist in database.");
	for (auto &user : data["users"])
	{
		// Verify fields
		LB_ASSERT(user["name"].is_string(), "User 'name' does not exist or is wrong type.");
		//LB_ASSERT(user["owned_games"].is_array(), "User's 'owned games' does not exist or is wrong type");

		// Create new user
		std::unique_ptr<User> u = std::make_unique<User>();
		u->name = user["name"];

		if (user.contains("owned_games"))
		{
			for (auto &game_name : user["owned_games"])
			{
				u->owned_games.push_back(game_name);
			}
		}
		user_database.insert({user["name"], std::move(u)});
	}
}

void Program::write_database()
{
	// Write json data
	std::ofstream f(data_path / "database_output.json");
	if (!f.is_open())
	{
		// Couldn't read json for some reason
		std::string error = "Could not open '" + data_path.string() + DATABASE_FILENAME + "'";
		LB_ASSERT(false, error.c_str());
		return;
	}

	json j;

	// -- Write games --
	{
		auto game_objects = json::array();
		for (auto &pair : game_database)
		{
			auto *game = pair.second.get();
			game_objects.push_back(
				{
					{"name", game->get_name() },
					{"description", game->get_description() },
					{"seller", game->get_seller_username() },
					{"price", game->get_price()}
				});
		}
		j["games"] = game_objects;
	}

	// -- Write users --
	{
		auto user_objects = json::array();
		for (auto &pair : user_database)
		{
			User *user = pair.second.get();

			auto owned_games = json::array();
			for (auto &game_name : user->owned_games)
			{
				owned_games.push_back(game_name);
			}
			user_objects.push_back(
				{
					{"name", user->name},
					{"owned_games", owned_games}
				});
		}
		j["users"] = user_objects;
	}

	f << std::setw(4) << j << std::endl;
	f.close();
}

void Program::clear_command()
{
	// Commands are heap allocated, so we need to free them here...
	for (Command* c : commands)
	{
		delete c;
	}
	commands.clear();
}

void Program::build_command_list()
{
	clear_command();
	if (!logged_in)
	{
		push_command<LoginCommand>();
	}
	else
	{
		push_command<LogoutCommand>();
		push_command<ListGamesCommand>();
	}

	push_command<ExitCommand>();
}
