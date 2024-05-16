#include "Log.h"
#include "Version.h"
#ifdef LUFT_PLATFORM_WINDOWS

extern Luft::Application* Luft::CreateApplication();

int main(int argc, char** argv)
{
	Luft::Log::Init();
	CORE_LOG_INFO("spdlog initialized");

	CORE_LOG_INFO("Luft ({0}) Startup", VERSIONSTR);
	// do something before loop

	CORE_LOG_INFO("Luft Run");
	auto app = Luft::CreateApplication();
	app->Run();

	CORE_LOG_INFO("Luft End");
	delete app;
	
}

#endif
