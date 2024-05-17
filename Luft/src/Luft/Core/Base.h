#pragma once
#include <memory>

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

#define ARRAYSIZE(arr) ((int)(sizeof(arr) / sizeof(*(arr))))

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Luft {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
