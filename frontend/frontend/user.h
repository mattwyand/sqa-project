#pragma once
#include <string>
#include <vector>

// Represents a user account along with the list of games the user owns.
class User
{
	// 'Program' class should be able to edit private members
	friend class Program;

public:
	// The four account types. The type decides which transactions the user may perform:
	// Admin        (AA) - every transaction, including privileged ones (create, delete, refund)
	// FullStandard (FS) - buy, sell, addcredit, list
	// BuyStandard  (BS) - buy, addcredit, list (cannot sell)
	// SellStandard (SS) - sell, addcredit, list (cannot buy)
	enum class Type
	{
		None, // only used when a type string could not be understood
		Admin,
		FullStandard,
		BuyStandard,
		SellStandard,
	};

private:
	// The user's name (at most 15 characters)
	std::string name = "";

	// The account type (see User::Type above)
	Type type = Type::None;

	// Names of the games this user owns (from the games collection file and purchases)
	std::vector<std::string> owned_games{};

	// The user's available credit in dollars
	double wallet = 0.0;

public:
	// Get the user's name
	inline const std::string &get_name() const { return name; }

	// Get the user's type
	inline User::Type get_type() const { return type; }

	// Get the list of the user's owned games
	inline const std::vector<std::string> &get_owned_games() const { return owned_games; }

	// Get the user's available credit
	inline double get_wallet_balance() const { return wallet; }

	// Does the user own a game with this name?
	bool owns_game(const std::string &game_name) const;

	// Convert a two-letter file code ("AA", "FS", "BS", "SS", any case) to a User::Type.
	// Returns Type::None if the code is not valid.
	static Type code_to_type(const std::string &code);

	// Convert a User::Type to its two-letter file code ("AA", "FS", "BS", "SS")
	static std::string type_to_code(Type type);
};
