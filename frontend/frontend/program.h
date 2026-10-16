#pragma once
#include "commands.h"

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

	std::string username = "";

	std::vector<Command *> commands{};

	template <class T>
	void push_command();
	void clear_command();

public:
	void run();

	void print_startup();
	void print_mainmenu();

	void login(const std::string &name);
	void logout();
	void exit();
};

template<class T>
inline void Program::push_command()
{
	commands.emplace_back(new T(this));
}
