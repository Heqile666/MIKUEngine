#pragma once
#include "Core/CoreDefinitions.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace MIKU {
	class MIKU_API Log
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

#define MIKU_CORE_TRACE(...)  ::MIKU::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MIKU_CORE_INFO(...)  ::MIKU::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MIKU_CORE_WARN(...)  ::MIKU::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MIKU_CORE_ERROR(...)  ::MIKU::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MIKU_CORE_FATAL(...)  ::MIKU::Log::GetCoreLogger()->fatak(__VA_ARGS__)


#define MIKU_TRACE(...)  ::MIKU::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MIKU_INFO(...)  ::MIKU::Log::GetClientLogger()->info(__VA_ARGS__)
#define MIKU_WARN(...)  ::MIKU::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MIKU_ERROR(...)  ::MIKU::Log::GetClientLogger()->error(__VA_ARGS__)
#define MIKU_FATAL(...)  ::MIKU::Log::GetClientLogger()->fatak(__VA_ARGS__)