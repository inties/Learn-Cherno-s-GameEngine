#include"pch.h"
#include"Windows.h"
#include"Engine/platform/WindowWindows.h"
namespace Engine {
	Windows* Windows::Create(WindowsProps& props)
	{
		// 这里可以根据平台创建不同的 Windows 实现
		// 例如，如果是 Windows 平台，可以返回 Windows 类的实例
		// 如果是其他平台，可以返回相应的实现
		#ifdef ENGINE_PLATFORM_WINDOWS
				return new WindowWindows(props);
		#endif
		return nullptr; // 需要实现具体的 Windows 类
	}
}
