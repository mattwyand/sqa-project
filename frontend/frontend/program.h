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

	// User "database"
	std::unordered_map<std::string, std::unique_ptr<Game>> game_database{};
	// Games "database"
	std::unordered_map<std::string, std::unique_ptr<User>> user_database{};

	// Transaction log
	std::vector<std::string> transaction_log{};

	// Program state
	bool is_admin_session = false;
	bool logged_in = false;
	bool exit_program = false;

	// Active user
	User *active_user;
	
	std::vector<Command *> commands{};

	// Find the "data" directory containing our "database"
	void init_data_path();

	// Read data/database.json and construct database
	void read_database();
	// Write our database structures to json file
	void write_database();

	template <class T>
	void push_command();
	void clear_command();

	// Figure out what commands to show in the menu based on program state
	void build_command_list();

	// For create, delete, add credit, end of session
	void push_transaction_type1(const User *user, const std::string &transaction_code);

public:
	// Start the program
	void run();

	// Show the program logo and version
	void print_startup();

	// Print commands in command list along with their descriptions
	void print_main_menu();

	User *get_user(const std::string& name) const;
	Game *get_game(const std::string& name) const;
	const std::filesystem::path &get_data_path() const;

	// -- Commands --
	void list_owned_games();

	void login(const std::string &name);
	void logout();
	void exit();

	// Admin commands
	void create_user(const std::string &name, User::Type type);
	void delete_user(const std::string &name);
	void add_credit(const std::string &name, float amount);
};

// TODO: Let's just allocate each command once, and push pointers from that into "commands"...
template<class T>
inline void Program::push_command()
{
	commands.emplace_back(new T(this));
}
