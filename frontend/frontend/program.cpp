#include "program.h"

#include "commands.h"
#include "util.h"
#include "game.h"

#include <iostream>
#include <vector>

void Program::run()
{
	init_game_list();
	init_users_list();

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
		for (Command* command : commands)
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
		std::cout << "-- Menu (" << user->get_name() << ") --\n";
	}
	else
	{
		std::cout << "-- Menu --\n";
	}

	for (Command* command : commands)
	{
		command->print_description();
	}

	std::cout << "\n>";
}

User *Program::get_user(const std::string &name) const
{
	if (user_database.count(name))
	{
		return user_database.at(name);
	}
	else
	{
		return nullptr;
	}
}

Game *Program::get_game(const std::string& name) const
{
	if (game_database.count(name))
	{
		return game_database.at(name);
	}
	else
	{
		return nullptr;
	}
}

void Program::list_owned_games()
{
	for (auto& game : user->get_owned_games())
	{
		std::cout << "\"" << game->get_name() << "\" - " << game->get_description() << "\n"
			<< "Seller: " << game->get_seller_username() << "\n"
			<< "Price: $" << game->get_price() << "\n\n";
	}
}

void Program::login(const std::string& name)
{
	user = get_user(name);
	if (!user)
	{
		std::cout << "User '" << name << "' does not exist.\n";
		return;
	}

	logged_in = true;
	std::cout << "Logged in as '" << name << "'\n";

	// Check if we're an admin
	if (user->get_name() == "admin")
	{
		std::cout << "Began Admin session. Privileged/unprivileged transactions are available.\n";
	}
}

void Program::logout()
{
	logged_in = false;
	user = nullptr;
}

void Program::exit()
{
	exit_program = true;
	std::cout << "Closing program...\n";
}

void Program::init_game_list()
{
	auto register_game = [this](
		const std::string& name,
		const std::string& desc,
		const std::string& seller,
		float price)
		{
			this->game_database.insert({
				name,
				new Game(name, desc, seller, price)
				});
		};

	register_game(
		"game1",
		"This is game 1",
		"Nintendo",
		999.0f);

	register_game(
		"game2",
		"This is game 2",
		"Bethesda",
		2.0f);
}

void Program::init_users_list()
{
	auto register_user = [this](
		const std::string name,
		const std::vector<Game*>& owned_games
		)
		{
			User* u = new User();
			u->name = name;
			u->owned_games = owned_games;
			user_database.insert({ name, u });
		};

	register_user("admin", {
		get_game("game1"),
		get_game("game2"),
		});

	register_user("carpet", {
		get_game("game1"),
		});
}

void Program::clear_command()
{
	// Commands are heap allocated, so we need to free them here...
	for (Command* c : commands)
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
