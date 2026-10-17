#include "commands.h"

#include "program.h"

#include <iostream>

void Command::print_description()
{
	std::cout << name << "\t- " << description << "\n";
}


// -- Exit --
ExitCommand::ExitCommand(Program *program)
	: Command(program, "exit", "exit the program") {}

void ExitCommand::run(const std::vector<std::string> &args)
{
	program->exit();
}


// -- Login --
LoginCommand::LoginCommand(Program *program)
	: Command(program, "login", "log into your account") {}

void LoginCommand::run(const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		std::cout << "usage: [name]\n";
		return;
	}

	program->login(args[1]);
}


// -- Logout --
LogoutCommand::LogoutCommand(Program *program)
	: Command(program, "logout", "log out of your account") {}

void LogoutCommand::run(const std::vector<std::string> &args)
{
	std::cout << "Logged out\n";
	program->logout();
}


// -- List games --
ListGamesCommand::ListGamesCommand(Program* program)
	: Command(program, "list", "list your owned games") {}

void ListGamesCommand::run(const std::vector<std::string>& args)
{
	std::cout << "-- Owned games --\n";
	program->list_owned_games();
}
