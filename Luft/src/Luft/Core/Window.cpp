#include "Window.h"

#ifdef LUFT_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Luft
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef LUFT_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
	#else
		return nullptr;
	#endif
	}

}