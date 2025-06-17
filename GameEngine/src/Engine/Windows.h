#pragma once
#include"pch.h"
#include"Event.h"
namespace Engine{
	class ENGINE_API Windows {
		struct WindowsProps {
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			WindowsProps(
				const std::string& title="myGameEngine",
				int wid=500, int heigh=500) :Title(title),Width(wid), Height(heigh) {};
		};

		virtual ~Windows() {

		}
		using EventFuc = std::function<void(Event&)>;
		virtual void Update()=0;
		virtual void SetEventCallBack(const EventFuc& callback)=0;
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		static Windows* Create(WindowsProps& props = WindowsProps());

	};
}