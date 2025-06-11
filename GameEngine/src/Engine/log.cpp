#include "log.h"

namespace Engine {
    // 定义静态成员变量
    std::shared_ptr<spdlog::logger> Log::s_Client_logger;
    std::shared_ptr<spdlog::logger> Log::s_Engine_logger;
}
