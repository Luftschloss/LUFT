#include "EnumDefs.h"

namespace Luft
{
	//locatization
	std::optional<char const*> GetTextVal(TextKey);
	//resource
	std::optional<char const*> GetResVal(ResKey);
	//
	std::optional<int> GetIntVal(FontConfigKey);
}