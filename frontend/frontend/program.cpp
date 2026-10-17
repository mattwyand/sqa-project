#include "program.h"

#include "commands.h"
#include "util.h"
#include "game.h"

#include <iostream>
#include <vector>

void Program::run()
{
	print_startup();

	while (!exit_program)
	{
		build_command_list();
		print_mainmenu();

		// Get input
		std::string input;
		std::getline(std::cin, input);

		// TODO: strip input of whitespace

		// Transform to lowercase so input can be any case
		input = to_lower_case(input);

		// See if we input one of the commands listed
		auto args = split(input, ' ');
		for (Command *command : commands)
		{
			if (args[0] == command->get_name())
			{
				command->run(args);
				break;
			}
		}

		std::cout << "\n\n";
	}
}

void Program::print_startup()
{
	std::cout << " _   _       _       ____  _                       \n"
		<< "| \\ | | ___ | |_    / ___|| |_ ___  __ _ _ __ ___\n"
		<< "|  \\| |/ _ \\| __|___\\___ \\| __/ _ \\/ _` | '_ ` _ \\ \n"
		<< "| |\\  | (_) | ||_____|__) | ||  __/ (_| | | | | | |\n"
		<< "|_| \\_|\\___/ \\__|   |____/ \\__\\___|\\__,_|_| |_| |_|\n";
	std::cout << "Version 0.0.1\n\n\n";
}

void Program::print_mainmenu()
{
	if (logged_in)
	{
		// Print username in menu header if we're logged in
		std::cout << "-- Menu (" << user.get_name() << ") --\n";
	}
	else
	{
		std::cout << "-- Menu --\n";
	}

	for (Command *command : commands)
	{
		command->print_description();
	}

	std::cout << "\n>";
}

void Program::list_owned_games()
{
	for (auto &game : user.get_owned_games())
	{
		std::cout << "\"" << game.get_name() << "\" - " << game.get_description() << "\n"
			<< "Seller: " << game.get_seller_username() << "\n"
			<< "Price: $" << game.get_price() << "\n\n";
	}
}

void Program::login(const std::string &name)
{
	logged_in = true;
	user = get_user(name);

	std::cout << "Logged in as '" << name << "'\n";

	// Check if we're an admin
	if (user.get_name() == "admin")
	{
		std::cout << "Began Admin session. Privileged/unprivileged transactions are available.\n";
	}
}

void Program::logout()
{
	logged_in = false;
	user = User(); // Reset to default user
}

void Program::exit()
{
	exit_program = true;
	std::cout << "Closing program...\n";
}

void Program::clear_command()
{
	// Commands are heap allocated, so we need to free them here...
	for (Command *c : commands)
	{
		delete c;
	}
	commands.clear();
}

void Program::build_command_list()
{
	clear_command();
	if (!logged_in)
	{
		push_command<LoginCommand>();
	}
	else
	{
		push_command<LogoutCommand>();
		push_command<ListGamesCommand>();
	}

	push_command<ExitCommand>();
}
