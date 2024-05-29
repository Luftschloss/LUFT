#include "SystemService.h"

#define RES_FONT_PATH	"resources/fonts/"

namespace Luft
{

	std::optional<char const*> GetResVal(ResKey e)
	{
		switch (e)
		{
		case ResKey::font_path_puhui3:
			return RES_FONT_PATH "AlibabaPuHuiTi-3-55-Regular.ttf";
		}
		return std::optional<char const*>();
	}

	std::optional<int> GetIntVal(IntKey e)
	{
		switch (e)
		{
		case IntKey::font_size_max:
			return 25;
		case IntKey::font_size_normal:
			return 18;
		case IntKey::font_size_title:
			return 30;
		}
		return -1;
	}
}