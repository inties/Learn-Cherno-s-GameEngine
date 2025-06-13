#pragma once
#include"Engine/core.h"
#include<string>
#include<iostream>
#include <utility> 
#include<sstream>
namespace Engine{
	enum EventType {
		None = 0,
		MouseMove,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased
	};
	enum EventCategory {
		KeyEvent = 1 << 0,
		MouseEvent = 1 << 1,
		WindowEvent = 1 << 2,
	};
	class ENGINE_API Event {
	public:
		virtual EventType getEventType() const = 0;
		virtual EventCategory getCategoryFlags() const = 0;
		virtual  const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		inline bool isInCategory(EventCategory category) const {
			return getCategoryFlags() & category;
		}
	};
#define EVENT_CLASS_TYPE(type) \
		static EventType getStaticType() {  return EventType::##type; } \
		virtual EventType getEventType() const override { return getStaticType(); } \
		virtual const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
	class ENGINE_API MouseMoveEvent : public Event {
	public:
		MouseMoveEvent(double x, double y) :pos_x(x), pos_y(y) {

		}
		std::pair<double, double>getMousePos() {
			return std::make_pair(pos_x, pos_y);
		}
		static EventType getStaticType() { return EventType::MouseMove; }
		EventType getEventType() const override {
			return getStaticType();
		}
		const char* GetName() const override {
			return "MouseMove";
		}
		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseMove: " << pos_x << ", " << pos_y;
			return ss.str();

		}

		EventCategory getCategoryFlags() const override {
			return EventCategory::MouseEvent;
		}
	private:
		double pos_x, pos_y;
	};
}
//class ENGINE_API KeyPressEvent : public Event {
//public:
//	KeyPressEvent(double m_KeyCode, int m_repeatCount) :keyCode(m_KeyCode), repeatCount(m_repeatCount) {
//
//	}
//	static EventType getStaticType() { return EventType::KeyPressed; }
//	EventType getEventType() const override {
//		return getStaticType();
//	}
//	const char* GetName() const override {
//		return "KeyPressed";
//	}
//	std::string ToString() const override {
//		return "KeyPressed: " + std::to_string(keyCode) + " Repeat Count: " + std::to_string(repeatCount);
//	}
//	EventCategory getCategoryFlags() const override {
//		return EventCategory::KeyEvent;
//	}
//private:
//	int keyCode, repeatCount;
//};
//
//}

