#pragma once
#include <atomic>
#include "Base.h"
#include "Window.h"
#include "LayerStack.h"
#include "Luft/ImGui/ImGuiLayer.h"
#include "Luft/Events/ApplicationEvents.h"

int main(int argc, char** argv);

namespace Luft
{
	class LUFT_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

	public:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PushEvent(Event& eve)
		{
			OnEvent(eve);
		}
		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }
	private:
		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		static Application* s_Instance;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

	private:
		std::atomic<bool> m_running = false;
		std::atomic<bool> m_windowFocused = false;
		double m_lastFrameTime = 0;
		
	};

	Application* CreateApplication();
}