#include "program.h"

#include "commands.h"
#include "util.h"
#include "game.h"

#include "thirdparty/json.hpp"
#include <iostream>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace
{
	const std::string DATABASE_FILENAME = "database.json";
	const std::string TRANSACTION_LOG_FILENAME = "transaction_log.txt";

	const std::string ALL_GAMES = "__all__";

	constexpr int USERNAME_MAX_LENGTH = 15;
	constexpr int MAX_CREDIT_DIGITS = 9;
}

void Program::run()
{
	init_data_path();
	read_database();
	write_database();

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
		if (args.size() > 0)
		{
			std::cout << "\n";

			bool found_command = false;
			for (Command *command : commands)
			{
				if (args[0] == command->get_name())
				{
					command->run(args);
					found_command = true;
					break;
				}
			}

			if (!found_command)
			{
				std::cout << "'" << args[0] << "' is not an available command.\n";
			}
		}

		std::cout << "\n";
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
		std::cout << "-- Menu (" << active_user->get_name() << ") --\n";
	}
	else
	{
		std::cout << "-- Menu --\n";
	}

	for (Command *command : commands)
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

Game *Program::get_game(const std::string &name) const
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
	std::cout << active_user->get_name() << "'s games:\n";

	for (auto &game_name : active_user->get_owned_games())
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

void Program::login(const std::string &name)
{
	active_user = get_user(name);
	if (!active_user)
	{
		std::cout << "User '" << name << "' does not exist.\n";
		return;
	}

	logged_in = true;
	std::cout << "Logged in as '" << name << "'\n";

	// Check if we're an admin
	if (active_user->get_type() == User::Type::Admin)
	{
		std::cout << "Began Admin session. Privileged/unprivileged transactions are available.\n";
		is_admin_session = true;
	}
}

void Program::logout()
{
	push_transaction_type1(active_user, "00");
	
	logged_in = false;
	is_admin_session = false;
	active_user = nullptr;

	// Write transaction log
	fs::path path = data_path / TRANSACTION_LOG_FILENAME;
	std::ofstream f(path);

	if (!f.is_open())
	{
		std::cout << "Error: Could not write to transaction log ('" << path.string() << "')\n";
		return;
	}

	f << "--- Transaction log ---\n";
	for (auto &line : transaction_log)
	{
		f << line;
	}

	f.close();
}

void Program::create_user(const std::string &name, User::Type type)
{
	// Check if user exists
	if (user_database.count(name))
	{
		std::cout << "Failed to create account: user '" << name << "' already exists.\n";
		return;
	}

	// Can't be over certain length (because of transaction log)
	if (name.size() > USERNAME_MAX_LENGTH)
	{
		std::cout << "Failed to create account: username '" << name << "' is too long (max length:" << USERNAME_MAX_LENGTH << ")\n";
		return;
	}

	{
		auto new_user = std::make_unique<User>();
		new_user->name = name;
		new_user->type = type;
		user_database.insert({new_user->name, std::move(new_user)});
	}

	std::cout << "Created new user '" << name << "'.\n";

	// Save changes
	write_database();

	// Transaction log
	push_transaction_type1(get_user(name), "01");
}

void Program::delete_user(const std::string &name)
{
	// Check if user exists
	if (!user_database.count(name))
	{
		std::cout << "Failed to delete account: user '" << name << "' doesn't exist.\n";
		return;
	}

	// Transaction log
	push_transaction_type1(get_user(name), "02");
	
	// Delete the user from the database
	user_database.erase(name);

	std::cout << "Deleted user '" << name << "'\n";

	// Save changes
	write_database();
}

void Program::add_credit(const std::string &name, float amount)
{
	// Check if user exists
	if (!user_database.count(name))
	{
		std::cout << "Failed to add funds: user '" << name << "' doesn't exist.\n";
		return;
	}

	// Add funds to user
	User *user = get_user(name);
	user->wallet += amount;

	// Save changes
	write_database();

	// Transaction log
	push_transaction_type1(user, "06");

	std::cout << "Added $" << amount << " to " << name << "'s wallet (new balance: $" << user->get_wallet_balance() << ")\n";
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
		LB_ASSERT(game["name"].is_string(), "Game 'name' does not exist or is wrong type.");
		LB_ASSERT(game["description"].is_string(), "Game 'description' does not exist or is wrong type.");
		LB_ASSERT(game["seller"].is_string(), "Game 'seller' does not exist or is wrong type.");
		LB_ASSERT(game["price"].is_number(), "Game 'price' does not exist or is wrong type.");

		auto g = std::make_unique<Game>();
		g->name = game["name"].get<std::string>();
		g->description = game["description"].get<std::string>();
		g->seller_username = game["seller"].get<std::string>();
		g->price = game["price"].get<float>();

		game_database.insert({g->name,
							  std::move(g)});
	}

	// -- Users --
	LB_ASSERT(data["users"].is_array(), "'users' list does not exist in database.");
	for (auto &user : data["users"])
	{
		// Verify fields
		LB_ASSERT(user["name"].is_string(), "User 'name' does not exist or is wrong type.");
		// LB_ASSERT(user["owned_games"].is_array(), "User's 'owned games' does not exist or is wrong type");

		// Create new user
		std::unique_ptr<User> u = std::make_unique<User>();

		u->name = user["name"].get<std::string>();
		u->wallet = user["wallet"].get<float>();
		if (user.contains("owned_games"))
		{
			for (auto &game_name : user["owned_games"])
			{
				u->owned_games.push_back(game_name.get<std::string>());
			}
		}

		// Get type
		u->type = User::string_to_type(user["type"].get<std::string>());
		LB_ASSERT(u->type != User::Type::None, "User is missing type.");

		user_database.insert({user["name"], std::move(u)});
	}
}

void Program::write_database()
{
	// Write json data
	std::ofstream f(data_path / DATABASE_FILENAME);
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
				{{"name", game->get_name()},
				 {"description", game->get_description()},
				 {"seller", game->get_seller_username()},
				 {"price", game->get_price()}});
		}
		j["games"] = game_objects;
	}

	// -- Write users --
	{
		auto user_objects = json::array();
		for (auto &pair : user_database)
		{
			User *user = pair.second.get();

			// Owned games
			auto owned_games = json::array();
			for (auto &game_name : user->owned_games)
			{
				owned_games.push_back(game_name);
			}

			user_objects.push_back(
				{{"name", user->name},
				 {"type", User::type_to_string(user->type)},
				 {"wallet", user->wallet},
				 {"owned_games", owned_games}});
		}
		j["users"] = user_objects;
	}

	f << std::setw(4) << j << std::endl;
	f.close();
}

void Program::clear_command()
{
	// Commands are heap allocated, so we need to free them here...
	for (Command *c : commands)
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
		push_command<ExitCommand>();
	}
	else
	{
		push_command<ListGamesCommand>();

		// Add admin commands to the list
		if (is_admin_session)
		{
			push_command<CreateCommand>();
			push_command<DeleteCommand>();
			push_command<AddCreditCommand>();
		}
		push_command<LogoutCommand>();
	}
}

void Program::push_transaction_type1(const User *user, const std::string &transaction_code)
{
	std::string str = "";
	str += transaction_code + " " + user->get_name();

	// Pad username
	for (int i = 0; i < USERNAME_MAX_LENGTH - user->get_name().size(); i++)
		str += "";

	str += " ";

	// User type
	switch (active_user->type)
	{
	case User::Type::Admin:
		str += "AA";
		break;
	case User::Type::FullStandard:
		str += "FS";
		break;
	case User::Type::BuyStandard:
		str += "BS";
		break;
	case User::Type::SellStandard:
		str += "SS";
		break;
	}

	str += " ";

	// Available credit
	str += float_to_fmtstring(user->get_wallet_balance());
	str += "\n";

	transaction_log.push_back(str);
}