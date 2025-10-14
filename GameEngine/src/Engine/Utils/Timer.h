#pragma once
#include <GLFW/glfw3.h>
namespace Engine
{
	class GameTimer
	{
	public:
		static void Init()
		{
			glfwSetTime(0.0);
		}
		static void Tick()
		{	
			m_curTime = glfwGetTime();
			double delta = m_curTime - m_lastTime;
			if (delta > 0)
				m_FPS = 1.0f / delta;
			else
				m_FPS = 0;
			m_lastTime = m_curTime;
		}
		static inline float GetFPB() {
			double curTime = glfwGetTime();
			if (curTime - m_lastTime > 0.5)
				return 0.0f;
			else return m_FPS;
		}
		private :
		static double m_curTime;
		static double m_lastTime;
		static float m_FPS;	
	};

}
