#include "commands.h"

#include "program.h"

#include <iostream>

void Command::print_description()
{
	std::cout << name << "\t- " << description << "\n";
}


// -- Login --
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
void LogoutCommand::run(const std::vector<std::string> &args)
{
	std::cout << "Logged out\n";
	program->logout();
}


// -- List games --
void ListGamesCommand::run(const std::vector<std::string>& args)
{
	program->list_owned_games();
}


// -- Exit --
void ExitCommand::run(const std::vector<std::string> &args)
{
	program->exit();
}


// -- Create --
void CreateCommand::run(const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		std::cout << "usage: [name]\n";
		return;
	}
	
	program->create_user(args[1]);
}
