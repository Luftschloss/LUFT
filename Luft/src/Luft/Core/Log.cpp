#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Luft
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		// color [time] loger (source file line) logtext 
		// pattern detail see: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#customizing-format-using-set_pattern
		spdlog::set_pattern("%^[%T] %n %@: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Luft-Core");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("Luft-Client");
		s_CoreLogger->set_level(spdlog::level::trace);

	}
}
