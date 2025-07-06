#pragma once
#include"pch.h"
#include"core.h"
namespace Engine {
	class ENGINE_API Input {
	public:
		
		virtual bool IsKeyPressed(int keycode)=0;
		virtual bool IsMouseButtonPressed(int mousecode)=0;
		virtual double GetXpos()=0;
		virtual double GetYpos()=0;
		virtual std::pair<double,double> GetMousePos()=0;

	protected:
		static Input* s_Instance;//ͨ����̬�����������ҵ���ʼ����ʵ��
	public:
		static Input* GetInstance() {
			return s_Instance;
		}

	};
}