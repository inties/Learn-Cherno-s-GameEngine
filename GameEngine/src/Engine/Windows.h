#pragma once
#include"pch.h"
#include"Event.h"
#include <string>
#include <functional>
namespace Engine{
	class ENGINE_API Windows {
		
	public :
		using EventFuc = std::function<bool(Event&)>;
		struct WindowsProps {
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			EventFuc EventCallback;
			WindowsProps(
			const std::string& title="myGameEngine",
			int wid=500, int heigh=500) : Title(title), Width(wid), Height(heigh) {
		}
		};

		virtual ~Windows() {

		}
		
		virtual void Update()=0;
		virtual void SetEventCallBack(const EventFuc& callback)=0;
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		static Windows* Create(WindowsProps& props = WindowsProps());
	protected:
		WindowsProps m_Props;
	};
}