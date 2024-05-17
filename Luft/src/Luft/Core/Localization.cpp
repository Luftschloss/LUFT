#include "SystemService.h"

#define RCAST_CSTR(pointer) reinterpret_cast<const char*>(pointer)

namespace Luft
{
	std::optional<char const*> Luft::GetTextVal(TextKey e)
	{
		switch (e)
		{
		case TextKey::text_1:
			return RCAST_CSTR(u8"测试文本1");
		case TextKey::text_2:
			return RCAST_CSTR(u8"测试文本2");

		}
		return std::optional<char const*>();
	}
}