#pragma once
#include "commands.h"

#include <string>
#include <vector>
#include <memory>

// The main global state of the program

class Program
{
private:
	bool logged_in = false;
	std::string username = "";

	bool exit_program = false;

	std::vector<Command *> commands{};

	template <class T>
	void pushCommand();
	void clearCommands();

public:
	void run();

	void print_startup();
	void print_mainmenu();

	void login(const std::string &name);
	void logout();
	void exit();
};

template<class T>
inline void Program::pushCommand()
{
	commands.emplace_back(new T(this));
}
