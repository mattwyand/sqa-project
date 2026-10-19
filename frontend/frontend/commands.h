#pragma once
#include <string>
#include <vector>

class Program;

// Base class for all commands in the program
class Command
{
protected:
	Program *program;
	std::string name, description;

public:
	Command(Program *program, const char *name, const char *description)
		: program(program), name(name), description(description) {}

	virtual ~Command() = default;

	void print_description();
	virtual void run(const std::vector<std::string> &args) = 0;

	inline const std::string get_name() const { return name; }
};


/*
login - start a Front End session
	- Before processing a login transaction, the front end reads the current user accounts file.

	- Should ask for the username.

	- After the username is accepted, reads in the available games file,
	  and begins accepting transactions.
 */
class LoginCommand : public Command
{
public:
	LoginCommand(Program *program)
		: Command(program, "login", "log into your account") {}
	
	void run(const std::vector<std::string> &args) override;
};


/*
logout - end a Front End session
	- Should write out the daily transaction file and stop accepting any
	  transactions except login.
*/
class LogoutCommand : public Command
{
public:
	LogoutCommand(Program *program)
		: Command(program, "logout", "log out of your account") {}

	void run(const std::vector<std::string> &args) override;
};


/*
create - Creates a new user with buying and/or advertising privileges
	- Should ask for the new username (as a text line)

	- Then, should ask for the type of user (admin or full-standard,
	  buy-standard, sell-standard)

	- Should save this information to the daily transaction file
*/
class CreateCommand : public Command
{
public:
	CreateCommand(Program* program)
		: Command(program, "create", "Add a user with the ability to buy/sell games.") {}
		
	void run(const std::vector<std::string> &args) override;
};


/*
delete - Cancel any games for sale and remove the user account
	- Should ask for the username (as a text line)
	
	- Should save this information for the daily transaction file
*/
class DeleteCommand : public Command
{
public:
	DeleteCommand(Program *program)
		: Command(program, "delete", "Remove a user.") {}

	void run(const std::vector<std::string> &args) override;
};


/*
sell - Put a game up for sale
	- Should ask for the game name (as a text line)
	
	- Should ask for a price for the game in dollars (e.g. 15.00)

	- Should save this information to the daily transaction file
*/
class SellCommand : public Command
{
public:
	SellCommand(Program *program)
		: Command(program, "sell", "Put a game up for sale.") {}
		
	void run(const std::vector<std::string> &args) override;
};


/*
buy - purchase a game available for sale
	- Should ask for game name and the seller's username

	- Should add the new game to the user's collection
	
	- Should credit the seller with an amount equal to the game's price
	  and deduct that amount from the buyer

	- Should save this information to the daily transaction file
*/
class BuyCommand : public Command
{
public:
	BuyCommand(Program *program)
		: Command(program, "buy", "Purchase a game available for sale.") {}

	void run(const std::vector<std::string> &args) override;
};


/*
refund - Issue a credit to a buyer's account from a seller's account (privileged transaction)
	- Should ask for the buyer's username, seller's username, and the amount of
	  credit to transfer.

	- Should transfer the specified amount of credit from the seller's credit balance
	  to the buyer's credit balance
	
	- Should save this information for the daily transaction file
*/
class RefundCommand : public Command
{
public:
	RefundCommand(Program *program)
		: Command(program, "refund", "Issue a credit to a buyer's account from a seller's account") {}

	void run(const std::vector<std::string> &args) override;
};


/*
addcredit - Add credit into the system for the purchase of accounts
	- In admin mode, should ask for the amount of credit to add and the
	  username of the account to which the credit is being added

	- In a standard account, should ask for the amount of credit

	- Should save this information to the daily transaction file
*/
class AddCreditCommand : public Command
{
public:
	AddCreditCommand(Program *program)
		: Command(program, "addcredit", "Add credit to user.") {}

	void run(const std::vector<std::string> &args) override;
};


// NOTE: This might not be something we have to do?
class ListGamesCommand : public Command
{
public:
	ListGamesCommand(Program* program)
		: Command(program, "list", "List your owned games.") {}

	void run(const std::vector<std::string> &args) override;
};


/*
exit - exit the program
NOTE: This might not be something we have to do
*/
class ExitCommand : public Command
{
public:
	ExitCommand(Program *program)
		: Command(program, "exit", "exit the program") {}

	void run(const std::vector<std::string> &args) override;
};