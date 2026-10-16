#include "program.h"

#include "commands.h"
#include "util.h"

#include <iostream>
#include <vector>

void Program::run()
{
	print_startup();

	while (!exit_program)
	{
		// TEMP
		clearCommands();
		if (!logged_in)
		{
			pushCommand<LoginCommand>();
		}
		else
		{
			pushCommand<LogoutCommand>();
		}
		pushCommand<ExitCommand>();

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
	std::cout << "-- Menu --\n";

	for (Command *command : commands)
	{
		command->print_description();
	}
}

void Program::login(const std::string &name)
{
	logged_in = true;
	username = name;
}

void Program::logout()
{
	logged_in = false;
	username = "";
}

void Program::exit()
{
	exit_program = true;
}

void Program::clearCommands()
{
	for (Command *c : commands)
	{
		delete c;
	}
	commands.clear();
}