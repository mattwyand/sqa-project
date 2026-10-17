#pragma once
#include "commands.h"
#include "user.h"

#include <string>
#include <vector>
#include <memory>

// The main global state of the program

class Program
{
private:
	// Program state
	bool logged_in = false;
	bool exit_program = false;

	// Active user
	User user;
	
	std::vector<Command *> commands{};

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

	void list_owned_games();

	void login(const std::string &name);
	void logout();
	void exit();
};

template<class T>
inline void Program::push_command()
{
	commands.emplace_back(new T(this));
}
