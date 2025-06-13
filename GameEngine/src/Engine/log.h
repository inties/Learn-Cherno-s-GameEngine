#include"pch.h"
#pragma once
#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Engine {
	class ENGINE_API Log
	{
	public:
		static void init() {
			s_Client_logger = spdlog::stdout_color_mt("Client");
			s_Engine_logger = spdlog::stdout_color_mt("Engine");
			s_Client_logger->set_level(spdlog::level::trace);
			s_Engine_logger->set_level(spdlog::level::trace);
			spdlog::set_pattern("%^[%T] %n: %v%$");
		}
		inline static std::shared_ptr<spdlog::logger> getClientLogger() {
			return s_Client_logger;
		}
		inline static std::shared_ptr<spdlog::logger> getEngineLogger() {
			return s_Engine_logger;
		}
	
	private:
		static std::shared_ptr<spdlog::logger> s_Client_logger;
		static std::shared_ptr<spdlog::logger> s_Engine_logger;
	};
}
#define ENGINE_CORE_TRACE(...) ::Engine::Log::getEngineLogger()->trace(__VA_ARGS__)
#define ENGINE_CORE_INFO(...) ::Engine::Log::getEngineLogger()->info(__VA_ARGS__)
#define ENGINE_CORE_WARN(...) ::Engine::Log::getEngineLogger()->warn(__VA_ARGS__)
#define ENGINE_CORE_ERROR(...) ::Engine::Log::getEngineLogger()->error(__VA_ARGS__)

#define ENGINE_TRACE(...) ::Engine::Log::getClientLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) ::Engine::Log::getClientLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::Engine::Log::getClientLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::Engine::Log::getClientLogger()->error(__VA_ARGS__)
