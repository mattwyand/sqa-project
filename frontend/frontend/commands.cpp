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
	// @TODO: Allow for usernames with spaces

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

	// Type string constants
	const std::string admin_code 			= "aa";
	const std::string full_standard_code 	= "fs";
	const std::string buy_standard_code 	= "bs";
	const std::string sell_standard_code 	= "ss";

	// Figure out which type of user to create 
	const std::string &type_str = args[2];
	User::Type type = User::Type::None;

	if (type_str == admin_code)
	{
		type = User::Type::Admin;
	}
	else if (type_str == full_standard_code)
	{
		type = User::Type::FullStandard;
	}
	else if (type_str == buy_standard_code)
	{
		type = User::Type::BuyStandard;
	}
	else if (type_str == sell_standard_code)
	{
		type = User::Type::SellStandard;
	}
	else
	{
		// User entered invalid type...
		std::cout << "User type must be one of the following \n"
		<< admin_code << " - Admin\n"
		<< full_standard_code << " - Full Standard\n"
		<< buy_standard_code << " - Buy Standard\n"
		<< sell_standard_code << " - Sell Standard\n";
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
