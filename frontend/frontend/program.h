#pragma once
#include "commands.h"
#include "user.h"
#include "game.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// The main global state of the program

class Program
{
private:
	// User "database"
	std::unordered_map<std::string, Game*> game_database{};
	// Games "database"
	std::unordered_map<std::string, User*> user_database{};

	// Program state
	bool logged_in = false;
	bool exit_program = false;

	// Active user
	User *user;
	
	std::vector<Command *> commands{};

	// Initialize database of games in the store
	void init_game_list();

	//  Initialize database of users
	void init_users_list();

	template <class T>
	void push_command();
	void clear_command();

	// Figure out what commands to show in the menu based on program state
	void build_command_list();

public:
	// Start the program
	void run();

	// Show the program logo and version
	void print_startup();

	// Print commands in command list along with their descriptions
	void print_mainmenu();

	User *get_user(const std::string& name) const;
	Game *get_game(const std::string& name) const;

	// -- Commands --
	void list_owned_games();

	void login(const std::string &name);
	void logout();
	void exit();
};

// TODO: Let's just allocate each command once, and push pointers from that into "commands"...
template<class T>
inline void Program::push_command()
{
	commands.emplace_back(new T(this));
}
