#include"pch.h"
#pragma once
#include"Engine/core.h"

namespace Engine{
	enum EventType {
		None = 0,
		MouseMove, MouseButtonPressed, MouseButtonReleased,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased
	};
	enum EventCategory {
		NoneCategory = 0,
		KeyEvent = 1 << 0,
		MouseEvent = 1 << 1,
		WindowEvent = 1 << 2,
	};
	class ENGINE_API Event {
		friend class EventDispatcher; // 允许 EventDispatcher 访问私有成员
	public:
		virtual EventType getEventType() const = 0;
		virtual EventCategory getCategoryFlags() const = 0;
		virtual  const char* GetName() const = 0;
		inline bool isInCategory(EventCategory category) const {
			return getCategoryFlags() & category;
		}
	protected:
		bool m_handled = false; // 是否已经处理过事件
	};
#define EVENT_CLASS_TYPE(type) \
		static EventType getStaticType() {  return EventType::##type; } \
		EventType getEventType() const override { return getStaticType(); } \
		const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) EventCategory getCategoryFlags() const override { return EventCategory::##category; }
	//inline std::ostream& operator<<(std::ostream& os, const Event& e)
	//{
	//	return os << e.ToString();
	//}
	/// <summary>
	//-----------------------------------------------------------------------------
	// MouseMoveEvent 类表示鼠标移动事件，包含鼠标位置的 x 和 y 坐标。
	/// </summary>
	class ENGINE_API MouseMoveEvent : public Event {
	public:
		MouseMoveEvent(double x, double y) :pos_x(x), pos_y(y) {

		}
		std::pair<double, double>getMousePos() {
			return std::make_pair(pos_x, pos_y);
		}
		EVENT_CLASS_TYPE(MouseMove);
		EVENT_CLASS_CATEGORY(MouseEvent);
		std::string ToString() const{
			std::stringstream ss;
			ss << "MouseMove: " << pos_x << ", " << pos_y;
			return ss.str();
		}

	private:
		double pos_x, pos_y;
	};

	/// <summary>
	//-----------------------------------------------------------------------------
	// MouseButtonPressedEvent 类表示鼠标按钮按下事件
	/// </summary>
	class ENGINE_API MouseButtonPressedEvent : public Event {
	public:
		MouseButtonPressedEvent(int button) : m_Button(button) {}

		inline int getMouseButton() const { return m_Button; }
		std::string ToString() const {
			std::stringstream ss;
			ss << "MouseButtonPressed: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_CATEGORY(MouseEvent);
		EVENT_CLASS_TYPE(MouseButtonPressed);
		
	private:
		int m_Button;
	};

	/// <summary>
	//-----------------------------------------------------------------------------
	// MouseButtonReleasedEvent 类表示鼠标按钮释放事件
	/// </summary>
	class ENGINE_API MouseButtonReleasedEvent : public Event {
	public:
		MouseButtonReleasedEvent(int button) : m_Button(button) {}

		inline int getMouseButton() const { return m_Button; }
		std::string ToString() const {
			std::stringstream ss;
			ss << "MouseButtonReleased: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(MouseEvent);
		EVENT_CLASS_TYPE(MouseButtonReleased);
	private:
		int m_Button;
	};

	class ENGINE_API KeyPressEvent : public Event {
		public:
			KeyPressEvent(double m_KeyCode, int m_repeatCount) :keyCode(m_KeyCode), repeatCount(m_repeatCount) {

			}
			EVENT_CLASS_CATEGORY(KeyEvent);
			EVENT_CLASS_TYPE(KeyPressed);
			std::string ToString() const {
				return "KeyPressed: " + std::to_string(keyCode) + " Repeat Count: " + std::to_string(repeatCount);
			}
		
		private:
			int keyCode, repeatCount;
		};
	class ENGINE_API KeyReleaseEvent : public Event {
	public:
		KeyReleaseEvent(double m_KeyCode) :keyCode(m_KeyCode) {
		}
		EVENT_CLASS_CATEGORY(KeyEvent);
		EVENT_CLASS_TYPE(KeyReleased);
		std::string ToString() const {
			return "KeyReleased: " + std::to_string(keyCode);
		}
		
	private:
		int keyCode;
	};
	class ENGINE_API WindowCloseEvent :public Event {
	public:
		WindowCloseEvent() {
		};
		EVENT_CLASS_CATEGORY(WindowEvent);
		EVENT_CLASS_TYPE(WindowClose);

	private:
		
	};
	class ENGINE_API WindowResizeEvent :public Event {
	public:
		WindowResizeEvent(int wid,int height): m_Width(wid),m_Height(height){
		};
		EVENT_CLASS_CATEGORY(WindowEvent);
		EVENT_CLASS_TYPE(WindowResize);
		inline int GetWindowWidth() {
			return m_Width;
		}
		inline int GetWindowHeight() {
			return m_Height;
		}
	private:
		int m_Width;
		int m_Height;
	};
	class ENGINE_API EventDispatcher {//负责调用泛型编程的事件处理函数
	public:
		EventDispatcher(Event& event) :m_event(event) {}
		template<typename T>
		bool Dispatch(std::function<bool(T&)> func) {//检查泛型的类型是否匹配，并尝试进行强行类型转换
			if (T::getStaticType() == m_event.getEventType()) {
				// 如果类型匹配，则调用传入的函数
				//return func(static_cast<T&>(m_event));
				m_event.m_handled = func(*(T*)&m_event);
				return m_event.m_handled; // 返回事件是否被处理

			}
			return false;
		}
	protected:
		Event& m_event;
	};
}





