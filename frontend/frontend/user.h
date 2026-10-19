#pragma once
#include "game.h"

#include <string>
#include <vector>

// Represents a user along with their list of owned games, and possibly published games
class User
{
	friend class Program;
public:
	enum class Type
	{
		None,
		Admin,
		FullStandard,
		BuyStandard,
		SellStandard,
	};

private:
	std::string name = "";
	Type type;
	std::vector<std::string> owned_games{};
	float wallet = 0.0f;

public:
	
	inline const float get_wallet_balance() const { return wallet; }
	inline const std::string get_name() const { return name; }
	inline const std::vector<std::string>& get_owned_games() const { return owned_games; }
	inline const User::Type get_type() const { return type; }

	static std::string type_to_string(Type user_type);
	static Type string_to_type(const std::string &str);
};
