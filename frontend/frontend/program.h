#pragma once
#include "commands.h"
#include "user.h"
#include "game.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/*
The main state of the Front End.

Holds the data read from the three input files, the current session (who is logged in),
and every transaction made so far. The Command classes handle prompting; this class
checks the rules of each transaction, applies it, and records it for the daily
transaction file.
*/
class Program
{
private:
	// -- File paths given on the command line --
	std::string accounts_path;          // current user accounts file (read)
	std::string available_games_path;   // available games file (read)
	std::string games_collection_path;  // games collection file (read)
	std::string transaction_path;       // daily transaction file (written at each logout)

	// -- Data read from the files at each login --
	// All users, looked up by username
	std::unordered_map<std::string, std::unique_ptr<User>> users{};
	// Games for sale, kept in the same order as the available games file
	std::vector<std::unique_ptr<Game>> available_games{};

	// -- Session state --
	bool logged_in = false;          // is anyone logged in?
	bool is_admin_session = false;   // is the logged-in user an admin?
	User *active_user = nullptr;     // the logged-in user (nullptr when logged out)
	// Credit added to each account during this session (for the $1000.00 limit)
	std::map<std::string, long long> credit_added_cents{};
	// Games put up for sale this session (they cannot be bought until the next session)
	std::vector<std::string> games_sold_this_session{};

	// Every daily transaction line from every session so far, in order
	std::vector<std::string> transaction_lines{};

	// All transaction codes the Front End understands
	std::vector<std::unique_ptr<Command>> commands{};

	// Read the accounts, available games and games collection files.
	// Returns false (after printing an error) if a file cannot be opened.
	bool read_input_files();

	// Look up a user / game for sale by name. Return nullptr if not found.
	User *find_user(const std::string &name) const;
	Game *find_game(const std::string &name) const;

	// Is this game name already used, by a game in the file or one sold this session?
	bool game_name_in_use(const std::string &name) const;

	// Add a line to the daily transaction file in the formats from the requirements:
	// codes 00, 01, 02, 06:  XX UUUUUUUUUUUUUUU TT CCCCCCCCC
	void record_user_transaction(const std::string &code, const User &user, double credit);
	// code 05:               XX buyer(15) seller(15) CCCCCCCCC
	void record_refund(const std::string &buyer, const std::string &seller, double amount);
	// code 03:               XX game(26) seller(15) PPPPPP
	void record_sell(const std::string &game, const std::string &seller, double price);
	// code 04:               XX game(26) seller(15) buyer(15) PPPPPP
	void record_buy(const std::string &game, const std::string &seller, const std::string &buyer, double price);

	// Write every recorded transaction line to the daily transaction file
	void write_transaction_file();

public:
	// Run the Front End: process transactions from standard input until it ends.
	void run(const std::string &accounts_file, const std::string &games_file,
			 const std::string &collection_file, const std::string &transaction_file);

	// Print 'message' on its own line, then read and return the next input line (trimmed).
	// Returns "" when there is no more input.
	std::string prompt(const std::string &message);

	// -- Session information used by the commands --
	bool is_logged_in() const { return logged_in; }
	bool is_admin() const { return is_admin_session; }
	std::string active_user_name() const;
	User::Type active_user_type() const;

	// -- Checks done between prompts (print an error and return false if invalid) --
	// New username: 1-15 characters, not already used
	bool check_new_username(const std::string &name) const;
	// New game name: 1-25 characters, not already used
	bool check_new_game_name(const std::string &name) const;

	// -- Transactions (each prints a success or error message) --
	void login(const std::string &name);
	void logout();
	void create_user(const std::string &name, User::Type type);
	void delete_user(const std::string &name);
	void sell_game(const std::string &game_name, const std::string &price_text);
	void buy_game(const std::string &game_name, const std::string &seller_name);
	void refund(const std::string &buyer_name, const std::string &seller_name, const std::string &amount_text);
	void add_credit(const std::string &target_name, const std::string &amount_text);
	void list_available_games() const;
};
