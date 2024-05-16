#pragma once
#include "Luft/Core/Base.h"
#include "spdlog/spdlog.h"

namespace Luft
{
	class LUFT_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core Log Macros
#define CORE_LOG_TRACE(...)		::Luft::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)		::Luft::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WRAN(...)		::Luft::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)		::Luft::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOG_FATAL(...)		::Luft::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client Log Macros
#define LUFT_LOG_TRACE(...)		::Luft::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LUFT_LOG_INFO(...)		::Luft::Log::GetClientLogger()->info(__VA_ARGS__)
#define LUFT_LOG_WRAN(...)		::Luft::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LUFT_LOG_ERROR(...)		::Luft::Log::GetClientLogger()->error(__VA_ARGS__)
#define LUFT_LOG_FATAL(...)		::Luft::Log::GetClientLogger()->fatal(__VA_ARGS__)