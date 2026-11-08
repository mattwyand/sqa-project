#include "user.h"
#include "util.h"

#include <algorithm>

bool User::owns_game(const std::string &game_name) const
{
	return std::find(owned_games.begin(), owned_games.end(), game_name) != owned_games.end();
}

User::Type User::code_to_type(const std::string &code)
{
	// Accept the code in any letter case, e.g. "fs" or "FS"
	const std::string upper = to_upper_case(code);

	if (upper == "AA") return Type::Admin;
	if (upper == "FS") return Type::FullStandard;
	if (upper == "BS") return Type::BuyStandard;
	if (upper == "SS") return Type::SellStandard;

	// Not a valid type code
	return Type::None;
}

std::string User::type_to_code(Type type)
{
	switch (type)
	{
	case Type::Admin:        return "AA";
	case Type::FullStandard: return "FS";
	case Type::BuyStandard:  return "BS";
	case Type::SellStandard: return "SS";
	default:                 return "  "; // unknown type -> blank field
	}
}
