#pragma once
#include <SDL.h>
#include "Luft/Core/Window.h"

namespace Luft
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void* GetNativeWindow() const { return m_Window; }

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_WindowData.Width; }
		unsigned int GetHeight() const override { return m_WindowData.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		struct WindowData
		{
			lstr Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;

			WindowData() = default;
		};
		WindowData m_WindowData;

		SDL_Window* m_Window;
	};
}