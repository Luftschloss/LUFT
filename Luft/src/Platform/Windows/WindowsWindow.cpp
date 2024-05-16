#include <SDL_vulkan.h>
#include "WindowsWindow.h"
#include "Luft/Core/Log.h"


namespace Luft
{
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
	}

	bool WindowsWindow::IsVSync() const
	{
		return false;
	}

	int SDLEventWatcher(void* data, SDL_Event* event)
	{
		SDL_Window* window = (SDL_Window*)(data);
		if (event->window.windowID == SDL_GetWindowID(window))
		{
			switch (event->type)
			{
			case SDL_QUIT:

				break;
			case SDL_WINDOWEVENT:
				if (event->window.event == SDL_WINDOWEVENT_CLOSE)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_RESTORED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_RESIZED)
				{
				}
				else if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_MINIMIZED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_MOVED)
				{

				}
				else if (event->window.event == SDL_WINDOWEVENT_HIT_TEST)
				{

				}
				break;
			}
		}
		return 0;
	}

	static void VulkanSetup(SDL_Window* window)
	{
		uint32_t extensions_count = 0;
		const char** extensions = nullptr;
		SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, extensions);
		
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			CORE_LOG_ERROR("SDL_Init(): %s", SDL_GetError());
			return;
		}

		// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
		// Create window with Vulkan graphics context
		SDL_DisplayMode sdm0;
		SDL_GetCurrentDisplayMode(0, &sdm0);
		auto display0Width = sdm0.w;
		auto display0Height = sdm0.h;
		auto WindowDefaultWidth = display0Width * 0.8;
		auto WidnowDefaultHeight = display0Height * 0.8;
		auto WindowMinWidth = display0Width * 0.55;
		auto WindowMinHeight = display0Height * 0.55;
		// TODO :User define toobar need add flag :SDL_WINDOW_BORDERLESS
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		m_Window = SDL_CreateWindow("Dear ImGui SDL2+Vulkan example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowDefaultWidth, WidnowDefaultHeight, window_flags);
		if (m_Window == nullptr)
		{
			CORE_LOG_ERROR("SDL_CreateWindow(): %s", SDL_GetError());
			return;
		}
		SDL_SetWindowResizable(m_Window, SDL_TRUE);
		SDL_SetWindowMinimumSize(m_Window, WindowMinWidth, WindowMinHeight);
		// TODO :User define toobar
		//SDL_SetWindowHitTest(window, HitTestCallback, nullptr);
		SDL_AddEventWatch(SDLEventWatcher, this->m_Window);

		//Vulkan Setup
		VulkanSetup(m_Window);
	}

	void WindowsWindow::Shutdown()
	{
	}
}