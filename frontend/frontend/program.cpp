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
	// Current version of the program
	const std::string PROGRAM_VERSION = "0.0.1";
	
	// Filepath constants
	const std::string DATABASE_FILENAME = "database.json";
	const std::string TRANSACTION_LOG_FILENAME = "transaction_log.txt";

	const std::string ACCOUNTS_FILENAME = "accounts.txt";
	const std::string AVAILABLE_GAMES_FILENAME = "games.txt";
	const std::string GAMES_COLLECTION_FILENAME = "accounts.txt";

	// If a user has a game named this in their owned games, then they own ALL games
	const std::string ALL_GAMES = "__all__";

	// Max length of a user name, as designated by the transaction log
	constexpr int MAX_USERNAME_LENGTH = 15;
	// Max number of credit digits, as designated by the transaction log
	constexpr int MAX_CREDIT_DIGITS = 9;

	// The fixed length of lines in the accounts file
	constexpr int LINE_LENGTH_ACCOUNTS = 28;
	// The fixed length of lines in the games file
	constexpr int LINE_LENGTH_GAMES = 49;
	// The fixed length of lines in the game collection file
	constexpr int LINE_LENGTH_GAME_COLLECTION = 42;

	// Return a username padded with spaces for transactions logs
	std::string get_padded_username(const std::string &name)
	{
		std::string result = name;
		for (int i = 0; i < MAX_USERNAME_LENGTH - name.size(); i++)
		{
			result += " ";
		}

		return result;
	}

	// Get 0-padded numeric value
	std::string get_padded_credit(float credit)
	{
		std::string str = "";
		
		// -- Pad the beginning with 0's --

		// NOTE: Add 2 because we're going to have 2 decimal points
		int digit_count = count_digits(credit) + 2;
		for (int i = 0; i < MAX_CREDIT_DIGITS - digit_count; i++)
		{
			str += "0";
		}
		
		str += float_to_fmtstring(credit);
		
		return str;
	}

	// Convert User::Type to string to be used in files
	std::string serialize_user_type(const User::Type type)
	{
		switch (type)
		{
		case User::Type::Admin:
			return "AA";
		case User::Type::FullStandard:
			return "FS";
		case User::Type::BuyStandard:
			return "BS";
		case User::Type::SellStandard:
			return "SS";
		}
	}

	// Convert user type string to User::Type from file
	User::Type deserialize_user_type(const std::string &type_string)
	{
		if (type_string == "AA")
		{
			return User::Type::Admin;
		}
		else if (type_string == "FS")
		{
			return User::Type::FullStandard;
		}
		else if (type_string == "BS")
		{
			return User::Type::BuyStandard;
		}
		else if (type_string == "SS")
		{
			return User::Type::SellStandard;
		}
	}
}

void Program::run()
{
	// Initialize database
	init_data_path();
	read_database();
	write_database_old();

	// Show ascii logo art
	print_startup();

	// -- Main loop --
	while (!exit_program)
	{
		// Figure out what commands to display and print them
		build_command_list();
		print_main_menu();

		// Get input
		std::string input;
		std::getline(std::cin, input);

		// @TODO: strip input of whitespace

		// Transform to lowercase so input can be case-agnostic
		input = to_lower_case(input);

		// -- See if input matches one of the listed commands --
		// Split input on space character
		auto args = split(input, ' ');

		if (args.size() > 0)
		{
			std::cout << "\n";

			bool found_command = false;
			for (Command *command : commands)
			{
				if (args[0] == command->get_name())
				{
					// The user input a valid command -> pass input to its 'run' function
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
	std::cout << "Version " << PROGRAM_VERSION << "\n\n\n";
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

	// Print available commands
	for (Command *command : commands)
	{
		command->print_description();
	}

	std::cout << "\n>";
}

User *Program::get_user(const std::string &name) const
{
	// Only return user that exists
	if (user_database.count(name))
	{
		return user_database.at(name).get();
	}
	else
	{
		// User not found
		return nullptr;
	}
}

Game *Program::get_game(const std::string &name) const
{
	// Only return game that exists
	if (game_database.count(name))
	{
		return game_database.at(name).get();
	}
	else
	{
		// Game not found
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
	log_transaction_general(active_user, "00");

	// Clear active user
	logged_in = false;
	is_admin_session = false;
	active_user = nullptr;

	write_transaction_log();
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
	if (name.size() > MAX_USERNAME_LENGTH)
	{
		std::cout << "Failed to create account: username '" << name << "' is too long (max length:" << MAX_USERNAME_LENGTH << ")\n";
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
	write_database_old();

	// Transaction log
	log_transaction_general(get_user(name), "01");
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
	log_transaction_general(get_user(name), "02");
	
	// Delete the user from the database
	user_database.erase(name);

	std::cout << "Deleted user '" << name << "'\n";

	// Save changes
	write_database_old();
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
	write_database_old();

	// Transaction log
	log_transaction_general(user, "06");

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
		// @TODO: Create new database file
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

void Program::write_database_old()
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

void Program::write_database()
{
	// -- Write accounts --
	{
		std::ofstream f(data_path / ACCOUNTS_FILENAME);
		if (!f.is_open())
		{
			// Couldn't open stream for reading
			std::string error = "Could not open '" + data_path.string() + ACCOUNTS_FILENAME + "' for writing.";
			LB_ASSERT(false, error.c_str());
		}

		for (auto &pair : user_database)
		{
			User *user = pair.second.get();
			std::string line = "";

			line += get_padded_username(user->get_name());
			line += " ";

			line += get_padded_credit(user->get_wallet_balance());
			line += " ";
			
		}

		// @TODO: File ends with special user named "END" with an empty user type and the credit field empty

		
		f.close();
	}

	// -- Write available games --
	{
		std::ofstream f(data_path / AVAILABLE_GAMES_FILENAME);
		if (!f.is_open())
		{
			// Couldn't open stream for reading
			std::string error = "Could not open '" + data_path.string() + AVAILABLE_GAMES_FILENAME + "' for writing.";
			LB_ASSERT(false, error.c_str());
		}

		
		f.close();
	}

	// -- Write games collection --
	{
		std::ofstream f(data_path / GAMES_COLLECTION_FILENAME);
		if (!f.is_open())
		{
			// Couldn't open stream for reading
			std::string error = "Could not open '" + data_path.string() + GAMES_COLLECTION_FILENAME + "' for writing.";
			LB_ASSERT(false, error.c_str());
		}
		
		f.close();
	}
}

void Program::clear_commands()
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
	clear_commands();

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

void Program::log_transaction_general(const User *user, const std::string &transaction_code)
{
	std::string str = "";

	// Write transaction code
	str += transaction_code;
	str += " ";

	// Write username
	str += get_padded_username(user->get_name());
	str += " ";

	// Write user type
	str += serialize_user_type(user->get_type());
	str += " ";

	// Write available credit
	str += float_to_fmtstring(user->get_wallet_balance());
	str += "\n";

	transaction_log.push_back(str);
}

void Program::log_transaction_refund(const User *user, const Game *game)
{
	std::string str = "";

	// Write transaction code (refund)
	str += "05";
	str += " ";

	// Write buyer's username
	str += get_padded_username(user->get_name());
	str += " ";
	
	// Write seller's username
	str += get_padded_username(game->get_seller_username());
	str += " ";

	// Write refund credit
	str += float_to_fmtstring(game->get_price());

	transaction_log.push_back(str);
}

void Program::log_transaction_sell(const Game *game)
{
	std::string str = "";

	// Write transaction code (sell)
	str += "03";
	str += " ";

	// Write game's name
	str += game->get_name();
	str += " ";
	
	// Write seller's username
	str += get_padded_username(game->get_seller_username());
	str += " ";

	// Write game's price
	str += float_to_fmtstring(game->get_price());

	transaction_log.push_back(str);
}

void Program::log_transaction_buy(const User *user, const Game *game)
{
	std::string str = "";

	// Write transaction code (buy)
	str += "04";
	str += " ";

	// Write game's name
	str += game->get_name();
	str += " ";
	
	// Write seller's username
	str += get_padded_username(game->get_seller_username());
	str += " ";

	// Write buyer's username
	str += get_padded_username(user->get_name());
	str += " ";

	// Write game's price
	str += float_to_fmtstring(game->get_price());

	transaction_log.push_back(str);
}

void Program::write_transaction_log()
{
	fs::path path = data_path / TRANSACTION_LOG_FILENAME;
	std::ofstream f(path);

	if (!f.is_open())
	{
		// Failed to open the file stream
		std::cout << "Error: Could not write to transaction log ('" << path.string() << "')\n";
		return;
	}

	// Write file contents
	f << "--- Transaction log ---\n";
	for (auto &line : transaction_log)
	{
		f << line;
	}

	transaction_log.clear();
	transaction_log.shrink_to_fit();

	f.close();
}