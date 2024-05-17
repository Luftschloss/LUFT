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
}