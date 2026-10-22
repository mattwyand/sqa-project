#pragma once
#include "game.h"

#include <string>
#include <vector>

// Represents a user along with their list of owned games, and possibly published games.
class User
{
	friend class Program;
public:
	// Users can be any one of these types.
	enum class Type
	{
		None,
		Admin,
		FullStandard,
		BuyStandard,
		SellStandard,
	};

private:
	// The user's name
	std::string name = "";

	// @TODO: Remove None, write this comment more descriptively
	// The type of user that this is. Determines the commands that the user has access to.
	// None - Default type. A user in the database should never have this type.
	// Admin - Has full access to the system.
	// FullStandard - A regular user that can both buy and sell games. (needs verification)
	// BuyStandard - A regular user that can only buy games. (needs verification)
	// SellStandard - A regular user that can only sell games. (needs verification)
	Type type;

	// A list of game titles which ared owned by this user.
	std::vector<std::string> owned_games{};

	// The amount of money or "credit" this user has in their virtual wallet.
	float wallet = 0.0f;

public:
	
	// Get the user's name
	inline const std::string get_name() const { return name; }

	// Get the user's type
	inline const User::Type get_type() const { return type; }
	
	// Get the list of user's owned games
	inline const std::vector<std::string>& get_owned_games() const { return owned_games; }

	// Get the user's wallet balance
	inline const float get_wallet_balance() const { return wallet; }


	// Static helper for converting User::Type to a string.
	// User::Type::Admin; 			- "admin" 	
	// User::Type::FullStandard; 	- "full-standard"
	// User::Type::BuyStandard; 	- "buy-standard"
	// User::Type::SellStandard; 	- "sell-standard"
	static std::string type_to_string(Type user_type);

	// Static helper for converting a string into a User::Type.
	// "admin" 						- User::Type::Admin;
	// "full-standard" 				- User::Type::FullStandard;
	// "buy-standard" 				- User::Type::BuyStandard;
	// "sell-standard" 				- User::Type::SellStandard;
	static Type string_to_type(const std::string &str);
};
