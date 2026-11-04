#include "user.h"
#include "game.h"

namespace
{
	const std::string STR_ADMIN = "admin";
	const std::string STR_FULL_STANDARD = "full-standard";
	const std::string STR_BUY_STANDARD = "buy-standard";
	const std::string STR_SELL_STANDARD = "sell-standard";
}

std::string User::type_to_string(Type user_type)
{
	switch (user_type)
	{
	case Type::Admin: 			return STR_ADMIN;
	case Type::FullStandard: 	return STR_FULL_STANDARD; 
	case Type::BuyStandard: 	return STR_BUY_STANDARD; 
	case Type::SellStandard: 	return STR_SELL_STANDARD; 
	
	default:
		// Invalid type
		return "";
	}
}

User::Type User::string_to_type(const std::string &str)
{
	// NOTE: We can't use a switch statement here because string comparison
	// overrides the '==' operator.
	if (str == STR_ADMIN)
	{
		return Type::Admin;
	}
	else if (str == STR_FULL_STANDARD)
	{
		return Type::FullStandard;
	}
	else if (str == STR_BUY_STANDARD)
	{
		return Type::BuyStandard;
	}
	else if (str == STR_SELL_STANDARD)
	{
		return Type::SellStandard;
	}

    return Type::None;
}
