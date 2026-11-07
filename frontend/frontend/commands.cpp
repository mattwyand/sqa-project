#include "commands.h"

#include "program.h"
#include "user.h"

#include <iostream>

void LoginCommand::run()
{
	// No second login until the current session has logged out
	if (program->is_logged_in())
	{
		std::cout << "Error: already logged in.\n";
		return;
	}

	program->login(program->prompt("Enter username:"));
}

void LogoutCommand::run()
{
	program->logout();
	std::cout << "Logged out.\n";
}

void CreateCommand::run()
{
	if (!program->is_admin())
	{
		std::cout << "Error: create is a privileged transaction.\n";
		return;
	}

	// Check the name before asking for the type, so the user hears about a bad name at once
	std::string new_name = program->prompt("Enter new username:");
	if (!program->check_new_username(new_name))
		return;

	std::string type_code = program->prompt("Enter user type (AA, FS, BS, SS):");
	User::Type type = User::code_to_type(type_code);
	if (type == User::Type::None)
	{
		std::cout << "Error: user type must be AA, FS, BS or SS.\n";
		return;
	}

	program->create_user(new_name, type);
}

void DeleteCommand::run()
{
	if (!program->is_admin())
	{
		std::cout << "Error: delete is a privileged transaction.\n";
		return;
	}

	program->delete_user(program->prompt("Enter username to delete:"));
}

void SellCommand::run()
{
	if (program->active_user_type() == User::Type::BuyStandard)
	{
		std::cout << "Error: buy-standard accounts cannot sell games.\n";
		return;
	}

	// Check the name before asking for the price
	std::string game_name = program->prompt("Enter game name:");
	if (!program->check_new_game_name(game_name))
		return;

	std::string price_text = program->prompt("Enter price:");
	program->sell_game(game_name, price_text);
}

void BuyCommand::run()
{
	if (program->active_user_type() == User::Type::SellStandard)
	{
		std::cout << "Error: sell-standard accounts cannot buy games.\n";
		return;
	}

	std::string game_name = program->prompt("Enter game name:");
	std::string seller_name = program->prompt("Enter seller username:");
	program->buy_game(game_name, seller_name);
}

void RefundCommand::run()
{
	if (!program->is_admin())
	{
		std::cout << "Error: refund is a privileged transaction.\n";
		return;
	}

	std::string buyer_name = program->prompt("Enter buyer username:");
	std::string seller_name = program->prompt("Enter seller username:");
	std::string amount_text = program->prompt("Enter refund amount:");
	program->refund(buyer_name, seller_name, amount_text);
}

void AddCreditCommand::run()
{
	std::string amount_text = program->prompt("Enter amount:");

	// Admins choose the account; standard users always credit their own account
	std::string target_name = program->active_user_name();
	if (program->is_admin())
		target_name = program->prompt("Enter username:");

	program->add_credit(target_name, amount_text);
}

void ListCommand::run()
{
	program->list_available_games();
}
