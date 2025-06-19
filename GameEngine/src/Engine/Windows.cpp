#include"pch.h"
#include"Windows.h"
#include"WindowWindows.h"
namespace Engine {
	Windows* Windows::Create(WindowsProps& props)
	{
		// ������Ը���ƽ̨������ͬ�� Windows ʵ��
		// ���磬����� Windows ƽ̨�����Է��� Windows ���ʵ��
		// ���������ƽ̨�����Է�����Ӧ��ʵ��
		#ifdef ENGINE_PLATFORM_WINDOWS
				return new WindowWindows(props);
		#endif
		return nullptr; // ��Ҫʵ�־���� Windows ��
	}
}
