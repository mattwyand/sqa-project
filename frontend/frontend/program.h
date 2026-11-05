#pragma once
#include "commands.h"
#include "user.h"
#include "game.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>

// The main global state of the program

class Program
{
private:
	// Path to the "data" folder
	std::filesystem::path data_path;

	// Games "database"
	std::unordered_map<std::string, std::unique_ptr<Game>> game_database{};
	// User "database"
	std::unordered_map<std::string, std::unique_ptr<User>> user_database{};

	// Transaction log
	std::vector<std::string> transaction_log{};

	// Program state
	bool is_admin_session = false;
	bool logged_in = false;
	bool exit_program = false;

	// Active user
	User *active_user;
	
	// List of available commands to the user
	std::vector<Command *> commands{};

	// Find the "data" directory containing our "database"
	void init_data_path();

	// Read the database
	void read_database();
	
	// Write to the database	
	void write_database();

	// Make a command available to the user
	template <class T>
	void push_command();

	// Clear all available commands from the list of available commands to the user
	void clear_commands();

	// Figure out what commands to show in the menu based on program state
	void build_command_list();

	// Log a create, delete, add credit, or end of session
	void log_transaction_general(const User *user, const std::string &transaction_code);

	// Log a refund
	void log_transaction_refund(const User *user, const Game *game);

	// Log a sell
	void log_transaction_sell(const Game *game);

	// Log a buy
	void log_transaction_buy(const User *user, const Game *game);

	// Write all transactions to disk and flush transaction_log list
	void write_transaction_log();

public:
	// Start the program
	void run();

	// Show the program logo and version
	void print_startup();

	// Print commands in command list along with their descriptions
	void print_main_menu();

	// Get a user from the database based on their name. Returns nullptr if not found.
	User *get_user(const std::string& name) const;

	// Get a game from the database based on its name. Returns nullptr if not found.
	Game *get_game(const std::string& name) const;

	// Return the cached path to the "data" directory
	const std::filesystem::path &get_data_path() const;

	// -- Commands --
	// List all of the current user's owned games
	void list_owned_games();

	// Log a user in and set as current user
	void login(const std::string &name);

	// Log a user out, clear current user
	void logout();

	// Exit the program
	void exit();

	// @TODO:
	// void buy();
	// void sell();
	// void refund();

	// -- Admin commands --
	// Creates a new user
	void create_user(const std::string &name, User::Type type);

	// Deletes an existing user
	void delete_user(const std::string &name);

	// Add credit to an existing user's balance
	void add_credit(const std::string &name, float amount);
};

// TODO: Let's just allocate each command once, and push pointers from that into "commands"...
template<class T>
inline void Program::push_command()
{
	commands.emplace_back(new T(this));
}
