#include "user.h"
#include "game.h"

std::string User::type_to_string(Type user_type)
{
	if (user_type == User::Type::Admin)
		return "admin";
	if (user_type == User::Type::FullStandard)
		return "full-standard";
	if (user_type == User::Type::BuyStandard)
		return "buy-standard";
	if (user_type == User::Type::SellStandard)
		return "sell-standard";

	return "";
}

User::Type User::string_to_type(const std::string &str)
{
	if (str == "admin")
		return User::Type::Admin;
	else if (str == "full-standard")
		return User::Type::FullStandard;
	else if (str == "buy-standard")
		return User::Type::BuyStandard;
	else if (str == "sell-standard")
		return User::Type::SellStandard;

    return Type::None;
}
