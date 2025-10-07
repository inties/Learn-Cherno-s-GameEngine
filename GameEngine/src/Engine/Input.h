#pragma once
#include"pch.h"
#include"core.h"
namespace Engine {
	class ENGINE_API Input {
	public:
		
		virtual bool IsKeyPressed(int keycode)=0;
		virtual bool IsMouseButtonPressed(int mousecode)=0;
		virtual float GetXpos()=0;
		virtual float GetYpos()=0;
		virtual std::pair<float,float> GetMousePos()=0;

	protected:
		static Input* s_Instance;//通过静态变量保证单例的唯一性实例
		static Input* Create();
	public:
		inline static Input* GetInstance() {
			if (s_Instance == nullptr)
			{
				s_Instance = Create();
			}
			return s_Instance;
		}

	};
}