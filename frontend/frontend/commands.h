#pragma once
#include <string>

class Program;

/*
Base class for every transaction code the Front End accepts.

Each command handles the user-interface side of one transaction: it checks whether the
logged-in user may perform it, prompts for the extra lines of input it needs, and then
asks the Program to apply the transaction. All data rules (unique names, balances,
limits, ...) are checked by the Program.
*/
class Command
{
protected:
	// The program state this command acts on
	Program *program;
	// The transaction code typed by the user to run this command, e.g. "login"
	std::string name;

public:
	Command(Program *program, const char *name)
		: program(program), name(name) {}

	virtual ~Command() = default;

	// Get the transaction code of this command
	inline const std::string &get_name() const { return name; }

	// Run the transaction. Called only after the main loop has matched the code.
	virtual void run() = 0;
};

/*
login - start a Front End session
	- asks for the username
	- rejected if a user is already logged in
*/
class LoginCommand : public Command
{
public:
	LoginCommand(Program *program) : Command(program, "login") {}
	void run() override;
};

/*
logout - end a Front End session
	- writes out the daily transaction file
	- after this, only login is accepted
*/
class LogoutCommand : public Command
{
public:
	LogoutCommand(Program *program) : Command(program, "logout") {}
	void run() override;
};

/*
create - add a new user (privileged: admin only)
	- asks for the new username, then the account type (AA, FS, BS, SS)
	- username must be 1-15 characters and not already in use
*/
class CreateCommand : public Command
{
public:
	CreateCommand(Program *program) : Command(program, "create") {}
	void run() override;
};

/*
delete - remove a user and cancel their games for sale (privileged: admin only)
	- asks for the username
	- the user must exist and must not be the current user
*/
class DeleteCommand : public Command
{
public:
	DeleteCommand(Program *program) : Command(program, "delete") {}
	void run() override;
};

/*
sell - put a game up for sale (not allowed for buy-standard accounts)
	- asks for the game name, then the price
	- name must be 1-25 characters and unique; price must be 0.00 to 999.99
	- the new game cannot be bought until the next session
*/
class SellCommand : public Command
{
public:
	SellCommand(Program *program) : Command(program, "sell") {}
	void run() override;
};

/*
buy - purchase a game that is for sale (not allowed for sell-standard accounts)
	- asks for the game name, then the seller's username
	- the buyer must have enough credit and must not already own the game
*/
class BuyCommand : public Command
{
public:
	BuyCommand(Program *program) : Command(program, "buy") {}
	void run() override;
};

/*
refund - move credit from a seller back to a buyer (privileged: admin only)
	- asks for the buyer's username, the seller's username, and the amount
	- both users must exist and the seller must have enough credit
*/
class RefundCommand : public Command
{
public:
	RefundCommand(Program *program) : Command(program, "refund") {}
	void run() override;
};

/*
addcredit - add credit to an account
	- asks for the amount; an admin is then also asked for the username to credit,
	  a standard user always credits their own account
	- at most $1000.00 can be added to one account in a session
*/
class AddCreditCommand : public Command
{
public:
	AddCreditCommand(Program *program) : Command(program, "addcredit") {}
	void run() override;
};

/*
list - print every game currently available for sale (any logged-in user)
*/
class ListCommand : public Command
{
public:
	ListCommand(Program *program) : Command(program, "list") {}
	void run() override;
};
