#include "Application.h"
#include "Platform/Windows/WinUtils.h"
#include "Log.h"

namespace Luft
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		if (s_Instance != nullptr)
		{
			CORE_LOG_ERROR("Application already exists!");
		}
		else
		{
			s_Instance = this;
			m_Window = Window::Create(WindowProps("Luft-Editor"));
			m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
			m_ImGuiLayer = new ImGuiLayer();
			PushOverlay(m_ImGuiLayer);
			m_running = true;
			m_windowFocused = true;
		}
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_running)
		{
			float time = Time::GetTime();
			Timestep timestep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (m_windowFocused)
			{
				//Layer Logic Update
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);

				//Layer Render
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}

		//
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& e)
	{
		//EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		//dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}
}
