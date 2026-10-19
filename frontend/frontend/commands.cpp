#include "commands.h"

#include "program.h"
#include "util.h"

#include <iostream>

void Command::print_description()
{
	std::cout << name << "\t- " << description << "\n";
}

void LoginCommand::run(const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		std::cout << "usage: login [name]\n";
		return;
	}
	program->login(args[1]);
}

void LogoutCommand::run(const std::vector<std::string> &args)
{
	std::cout << "Logged out\n";
	program->logout();
}

void ListGamesCommand::run(const std::vector<std::string>& args)
{
	program->list_owned_games();
}

void ExitCommand::run(const std::vector<std::string> &args)
{
	program->exit();
}

void CreateCommand::run(const std::vector<std::string> &args)
{
	// @TODO: Turn this into more than one step
	if (args.size() < 3)
	{
		std::cout << "usage: create [name] [type]\n";
		return;
	}

	const std::string &type_str = args[2];
	User::Type type = User::Type::None;
	if (type_str == "aa")
		type = User::Type::Admin;
	else if (type_str == "fs")
		type = User::Type::FullStandard;
	else if (type_str == "bs")
		type = User::Type::BuyStandard;
	else if (type_str == "ss")
		type = User::Type::SellStandard;
	else
	{
		std::cout << "User type must be one of the following \n"
		<< "aa - Admin\n"
		<< "fs - Full Standard\n"
		<< "bs - Buy Standard\n"
		<< "ss - Sell Standard\n";
		return;
	}
	
	program->create_user(args[1], type);
}

void DeleteCommand::run(const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		std::cout << "usage: delete [name]\n";
		return;
	}

	program->delete_user(args[1]);
}

void AddCreditCommand::run(const std::vector<std::string> &args)
{
	if (args.size() < 3)
	{
		std::cout << "usage: addcredit [name] [amount]\n";
		return;
	}

	if (!is_float(args[2]))
	{
		std::cout << "[amount] must be a number\n";
		return;
	}
	program->add_credit(args[1], std::stof(args[2]));
}
