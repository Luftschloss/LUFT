#pragma once

#ifdef LUFT_PLATFORM_WINDOWS
#ifdef LUFT_BUILD_DLL
#define LUFT_API __declspec(dllexport)
#else
#define LUFT_API __declspec(dllimport)
#endif
#else
#error Luft only supports Windows!
#endif

#define BIT(x) (1<<x)
