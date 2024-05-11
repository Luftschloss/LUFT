#pragma once
#include <atomic>
#include "Core.h"


namespace Luft
{
	class LUFT_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

	private:
		std::atomic<bool> m_running;

	};

	Application* CreateApplication();
}