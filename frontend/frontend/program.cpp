#include "program.h"

#include "commands.h"
#include "util.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace
{
	// Field widths from the requirements (file formats)
	constexpr int USERNAME_WIDTH = 15;   // UUUUUUUUUUUUUUU
	constexpr int GAME_NAME_WIDTH = 26;  // game name field in the games files
	constexpr int CREDIT_WIDTH = 9;      // CCCCCCCCC, e.g. 000500.00
	constexpr int PRICE_WIDTH = 6;       // PPPPPP, e.g. 020.00

	// Limits from the requirements
	constexpr int MAX_GAME_NAME_LENGTH = 25;
	constexpr long long MAX_PRICE_CENTS = 99999;              // $999.99
	constexpr long long MAX_CREDIT_PER_SESSION_CENTS = 100000; // $1000.00
	constexpr long long MAX_BALANCE_CENTS = 99999999;         // $999999.99 (largest 9-char credit field)

	// Fixed line lengths of the input files
	constexpr size_t ACCOUNTS_LINE_LENGTH = 28;
	constexpr size_t AVAILABLE_GAMES_LINE_LENGTH = 49;
	constexpr size_t COLLECTION_LINE_LENGTH = 42;

	// Name used on the last line of every input file
	const std::string END_MARKER = "END";
}

// ---------------------------------------------------------------------------
// Main loop and input
// ---------------------------------------------------------------------------

void Program::run(const std::string &accounts_file, const std::string &games_file,
				  const std::string &collection_file, const std::string &transaction_file)
{
	accounts_path = accounts_file;
	available_games_path = games_file;
	games_collection_path = collection_file;
	transaction_path = transaction_file;

	// Every transaction code the Front End understands
	commands.push_back(std::make_unique<LoginCommand>(this));
	commands.push_back(std::make_unique<LogoutCommand>(this));
	commands.push_back(std::make_unique<CreateCommand>(this));
	commands.push_back(std::make_unique<DeleteCommand>(this));
	commands.push_back(std::make_unique<SellCommand>(this));
	commands.push_back(std::make_unique<BuyCommand>(this));
	commands.push_back(std::make_unique<RefundCommand>(this));
	commands.push_back(std::make_unique<AddCreditCommand>(this));
	commands.push_back(std::make_unique<ListCommand>(this));

	std::cout << "Welcome to the Game Distribution System.\n";

	// One transaction code per line, until the input ends
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::string input = trim(line);
		if (input.empty())
			continue; // ignore blank lines

		// Before login, only "login" is accepted
		std::string code = to_lower_case(input);
		if (!logged_in && code != "login")
		{
			std::cout << "Error: you must log in first.\n";
			continue;
		}

		auto match = std::find_if(commands.begin(), commands.end(),
								  [&](const std::unique_ptr<Command> &c) { return c->get_name() == code; });
		if (match == commands.end())
			std::cout << "Error: unknown transaction '" << input << "'.\n";
		else
			(*match)->run();
	}
}

std::string Program::prompt(const std::string &message)
{
	std::cout << message << "\n";
	std::string line;
	if (!std::getline(std::cin, line))
		return ""; // no more input
	return trim(line);
}

std::string Program::active_user_name() const
{
	return active_user ? active_user->get_name() : "";
}

User::Type Program::active_user_type() const
{
	return active_user ? active_user->get_type() : User::Type::None;
}

// ---------------------------------------------------------------------------
// Reading the input files
// ---------------------------------------------------------------------------

bool Program::read_input_files()
{
	users.clear();
	available_games.clear();

	// -- Current user accounts: UUUUUUUUUUUUUUU TT CCCCCCCCC --
	std::ifstream accounts_file(accounts_path);
	if (!accounts_file)
	{
		std::cout << "Error: could not open accounts file '" << accounts_path << "'.\n";
		return false;
	}
	std::string line;
	while (std::getline(accounts_file, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back(); // tolerate Windows line endings
		if (line.size() != ACCOUNTS_LINE_LENGTH)
			continue; // skip badly formed lines instead of crashing

		std::string name = trim(line.substr(0, USERNAME_WIDTH));
		if (name == END_MARKER)
			break;

		auto user = std::make_unique<User>();
		user->name = name;
		user->type = User::code_to_type(line.substr(16, 2));
		std::string credit = line.substr(19, CREDIT_WIDTH);
		user->wallet = is_number(credit) ? std::stod(credit) : 0.0;
		users[name] = std::move(user);
	}

	// -- Available games: IIIIIIIIIIIIIIIIIIIIIIIIII SSSSSSSSSSSSSSS PPPPPP --
	std::ifstream games_file(available_games_path);
	if (!games_file)
	{
		std::cout << "Error: could not open available games file '" << available_games_path << "'.\n";
		return false;
	}
	while (std::getline(games_file, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.size() != AVAILABLE_GAMES_LINE_LENGTH)
			continue;

		std::string name = trim(line.substr(0, GAME_NAME_WIDTH));
		if (name == END_MARKER)
			break;

		auto game = std::make_unique<Game>();
		game->name = name;
		game->seller_username = trim(line.substr(27, USERNAME_WIDTH));
		std::string price = line.substr(43, PRICE_WIDTH);
		game->price = is_number(price) ? std::stod(price) : 0.0;
		available_games.push_back(std::move(game));
	}

	// -- Games collection: IIIIIIIIIIIIIIIIIIIIIIIIII UUUUUUUUUUUUUUU --
	std::ifstream collection_file(games_collection_path);
	if (!collection_file)
	{
		std::cout << "Error: could not open games collection file '" << games_collection_path << "'.\n";
		return false;
	}
	while (std::getline(collection_file, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.size() != COLLECTION_LINE_LENGTH)
			continue;

		std::string game_name = trim(line.substr(0, GAME_NAME_WIDTH));
		if (game_name == END_MARKER)
			break;

		User *owner = find_user(trim(line.substr(27, USERNAME_WIDTH)));
		if (owner)
			owner->owned_games.push_back(game_name);
	}

	return true;
}

User *Program::find_user(const std::string &name) const
{
	auto it = users.find(name);
	return it == users.end() ? nullptr : it->second.get();
}

Game *Program::find_game(const std::string &name) const
{
	for (const auto &game : available_games)
	{
		if (game->get_name() == name)
			return game.get();
	}
	return nullptr;
}

bool Program::game_name_in_use(const std::string &name) const
{
	return find_game(name) != nullptr ||
		   std::find(games_sold_this_session.begin(), games_sold_this_session.end(), name) != games_sold_this_session.end();
}

// ---------------------------------------------------------------------------
// Checks done between prompts
// ---------------------------------------------------------------------------

bool Program::check_new_username(const std::string &name) const
{
	if (name.empty() || name.size() > USERNAME_WIDTH)
	{
		std::cout << "Error: username must be 1 to 15 characters.\n";
		return false;
	}
	if (find_user(name))
	{
		std::cout << "Error: user '" << name << "' already exists.\n";
		return false;
	}
	return true;
}

bool Program::check_new_game_name(const std::string &name) const
{
	if (name.empty() || name.size() > MAX_GAME_NAME_LENGTH)
	{
		std::cout << "Error: game name must be 1 to 25 characters.\n";
		return false;
	}
	if (game_name_in_use(name))
	{
		std::cout << "Error: a game named '" << name << "' already exists.\n";
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
// Transactions
// ---------------------------------------------------------------------------

void Program::login(const std::string &name)
{
	// The files are read at every login so each session starts from the current data
	if (!read_input_files())
		return;

	User *user = find_user(name);
	if (!user)
	{
		std::cout << "Error: user '" << name << "' does not exist.\n";
		return;
	}

	active_user = user;
	logged_in = true;
	is_admin_session = (user->get_type() == User::Type::Admin);
	credit_added_cents.clear();
	games_sold_this_session.clear();

	std::cout << "Logged in as " << name << ".\n";
}

void Program::logout()
{
	// End-of-session record, then save the daily transaction file
	record_user_transaction("00", *active_user, active_user->get_wallet_balance());
	write_transaction_file();

	logged_in = false;
	is_admin_session = false;
	active_user = nullptr;
}

void Program::create_user(const std::string &name, User::Type type)
{
	auto user = std::make_unique<User>();
	user->name = name;
	user->type = type;
	record_user_transaction("01", *user, 0.0);
	users[name] = std::move(user);

	std::cout << "User '" << name << "' created.\n";
}

void Program::delete_user(const std::string &name)
{
	User *user = find_user(name);
	if (!user)
	{
		std::cout << "Error: user '" << name << "' does not exist.\n";
		return;
	}
	if (user == active_user)
	{
		std::cout << "Error: you cannot delete the current user.\n";
		return;
	}

	record_user_transaction("02", *user, user->get_wallet_balance());

	// Cancel the deleted user's games for sale, then remove the account
	available_games.erase(std::remove_if(available_games.begin(), available_games.end(),
										 [&](const std::unique_ptr<Game> &g) { return g->get_seller_username() == name; }),
						  available_games.end());
	users.erase(name);

	std::cout << "User '" << name << "' deleted.\n";
}

void Program::sell_game(const std::string &game_name, const std::string &price_text)
{
	if (!is_number(price_text))
	{
		std::cout << "Error: price must be a number.\n";
		return;
	}
	double price = std::stod(price_text);
	if (to_cents(price) < 0 || to_cents(price) > MAX_PRICE_CENTS)
	{
		std::cout << "Error: price must be between 0.00 and 999.99.\n";
		return;
	}

	// Not added to available_games: it can't be bought until the next session
	games_sold_this_session.push_back(game_name);
	record_sell(game_name, active_user->get_name(), price);

	std::cout << "Game '" << game_name << "' listed for sale at $" << format_money(price) << ".\n";
}

void Program::buy_game(const std::string &game_name, const std::string &seller_name)
{
	Game *game = find_game(game_name);
	User *seller = find_user(seller_name);
	if (!game || !seller || game->get_seller_username() != seller_name)
	{
		std::cout << "Error: game '" << game_name << "' is not available from seller '" << seller_name << "'.\n";
		return;
	}
	if (active_user->owns_game(game_name))
	{
		std::cout << "Error: you already own '" << game_name << "'.\n";
		return;
	}
	if (to_cents(active_user->get_wallet_balance()) < to_cents(game->get_price()))
	{
		std::cout << "Error: insufficient credit to buy '" << game_name << "'.\n";
		return;
	}

	// Move the money and add the game to the buyer's collection
	active_user->wallet -= game->get_price();
	seller->wallet += game->get_price();
	active_user->owned_games.push_back(game_name);
	record_buy(game_name, seller_name, active_user->get_name(), game->get_price());

	std::cout << "Purchased '" << game_name << "' from " << seller_name << " for $" << format_money(game->get_price()) << ".\n";
}

void Program::refund(const std::string &buyer_name, const std::string &seller_name, const std::string &amount_text)
{
	User *buyer = find_user(buyer_name);
	User *seller = find_user(seller_name);
	if (!buyer)
	{
		std::cout << "Error: buyer '" << buyer_name << "' does not exist.\n";
		return;
	}
	if (!seller)
	{
		std::cout << "Error: seller '" << seller_name << "' does not exist.\n";
		return;
	}
	if (!is_number(amount_text))
	{
		std::cout << "Error: amount must be a number.\n";
		return;
	}
	double amount = std::stod(amount_text);
	if (to_cents(amount) <= 0)
	{
		std::cout << "Error: amount must be greater than 0.\n";
		return;
	}
	// Requirements gap (test 36): a seller's credit can never go below zero
	if (to_cents(seller->get_wallet_balance()) < to_cents(amount))
	{
		std::cout << "Error: seller '" << seller_name << "' does not have enough credit for this refund.\n";
		return;
	}

	seller->wallet -= amount;
	buyer->wallet += amount;
	record_refund(buyer_name, seller_name, amount);

	std::cout << "Refunded $" << format_money(amount) << " from " << seller_name << " to " << buyer_name << ".\n";
}

void Program::add_credit(const std::string &target_name, const std::string &amount_text)
{
	if (!is_number(amount_text))
	{
		std::cout << "Error: amount must be a number.\n";
		return;
	}
	double amount = std::stod(amount_text);
	if (to_cents(amount) <= 0)
	{
		std::cout << "Error: amount must be greater than 0.\n";
		return;
	}

	User *target = find_user(target_name);
	if (!target)
	{
		std::cout << "Error: user '" << target_name << "' does not exist.\n";
		return;
	}

	// At most $1000.00 per account per session
	long long already_added = credit_added_cents[target_name];
	if (already_added + to_cents(amount) > MAX_CREDIT_PER_SESSION_CENTS)
	{
		std::cout << "Error: a maximum of $1000.00 can be added to an account per session.\n";
		return;
	}
	if (to_cents(target->get_wallet_balance()) + to_cents(amount) > MAX_BALANCE_CENTS)
	{
		std::cout << "Error: balance cannot be more than $999999.99.\n";
		return;
	}

	target->wallet += amount;
	credit_added_cents[target_name] = already_added + to_cents(amount);
	record_user_transaction("06", *target, amount);

	std::cout << "Added $" << format_money(amount) << " to " << target_name
			  << ". New balance: $" << format_money(target->get_wallet_balance()) << ".\n";
}

void Program::list_available_games() const
{
	std::cout << "Games available for sale:\n";
	if (available_games.empty())
		std::cout << "(none)\n";

	for (const auto &game : available_games)
	{
		std::cout << game->get_name() << " | " << game->get_seller_username()
				  << " | $" << format_money(game->get_price()) << "\n";
	}
}

// ---------------------------------------------------------------------------
// Daily transaction file
// ---------------------------------------------------------------------------

void Program::record_user_transaction(const std::string &code, const User &user, double credit)
{
	transaction_lines.push_back(code + " " + pad_right(user.get_name(), USERNAME_WIDTH) + " " +
								User::type_to_code(user.get_type()) + " " + format_money_padded(credit, CREDIT_WIDTH));
}

void Program::record_refund(const std::string &buyer, const std::string &seller, double amount)
{
	transaction_lines.push_back("05 " + pad_right(buyer, USERNAME_WIDTH) + " " + pad_right(seller, USERNAME_WIDTH) +
								" " + format_money_padded(amount, CREDIT_WIDTH));
}

void Program::record_sell(const std::string &game, const std::string &seller, double price)
{
	transaction_lines.push_back("03 " + pad_right(game, GAME_NAME_WIDTH) + " " + pad_right(seller, USERNAME_WIDTH) +
								" " + format_money_padded(price, PRICE_WIDTH));
}

void Program::record_buy(const std::string &game, const std::string &seller, const std::string &buyer, double price)
{
	transaction_lines.push_back("04 " + pad_right(game, GAME_NAME_WIDTH) + " " + pad_right(seller, USERNAME_WIDTH) +
								" " + pad_right(buyer, USERNAME_WIDTH) + " " + format_money_padded(price, PRICE_WIDTH));
}

void Program::write_transaction_file()
{
	// The file holds every session of the day, so it is rewritten in full at each logout
	std::ofstream file(transaction_path);
	if (!file)
	{
		std::cout << "Error: could not write daily transaction file '" << transaction_path << "'.\n";
		return;
	}
	for (const std::string &line : transaction_lines)
		file << line << "\n";
}
