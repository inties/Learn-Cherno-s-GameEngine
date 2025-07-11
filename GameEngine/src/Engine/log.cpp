#include"pch.h"
#include "log.h"
#include "spdlog/spdlog.h"

namespace Engine {
    // 显式实例化并导出模板，确保spdlog::logger的shared_ptr实现被导出
    template class ENGINE_API std::shared_ptr<spdlog::logger>;
    
    // 定义静态成员变量 - ENGINE_API在头文件中已声明
    std::shared_ptr<spdlog::logger> Log::s_Client_logger;
    std::shared_ptr<spdlog::logger> Log::s_Engine_logger;
}
