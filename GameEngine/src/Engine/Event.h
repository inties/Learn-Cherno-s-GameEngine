#include"pch.h"
#pragma once
#include"Engine/core.h"

namespace Engine{
	enum EventType {
		None = 0,
		MouseMove,
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
		friend class EventDispatcher; // ���� EventDispatcher ����˽�г�Ա
	public:
		virtual EventType getEventType() const = 0;
		virtual EventCategory getCategoryFlags() const = 0;
		virtual  const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		inline bool isInCategory(EventCategory category) const {
			return getCategoryFlags() & category;
		}
	protected:
		bool m_handled = false; // �Ƿ��Ѿ�������¼�
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
	/// <summary>
	//-----------------------------------------------------------------------------
	// MouseMoveEvent ���ʾ����ƶ��¼����������λ�õ� x �� y ���ꡣ
	/// </summary>
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
	class ENGINE_API KeyPressEvent : public Event {
		public:
			KeyPressEvent(double m_KeyCode, int m_repeatCount) :keyCode(m_KeyCode), repeatCount(m_repeatCount) {

			}
			static EventType getStaticType() { return EventType::KeyPressed; }
			EventType getEventType() const override {
				return getStaticType();
			}
			const char* GetName() const override {
				return "KeyPressed";
			}
			std::string ToString() const override {
				return "KeyPressed: " + std::to_string(keyCode) + " Repeat Count: " + std::to_string(repeatCount);
			}
			EventCategory getCategoryFlags() const override {
				return EventCategory::KeyEvent;
			}
		private:
			int keyCode, repeatCount;
		};
	class ENGINE_API KeyReleaseEvent : public Event {
	public:
		KeyReleaseEvent(double m_KeyCode) :keyCode(m_KeyCode) {
		}
		static EventType getStaticType() { return EventType::KeyReleased; }
		EventType getEventType() const override {
			return getStaticType();
		}
		const char* GetName() const override {
			return "KeyReleased";
		}
		std::string ToString() const override {
			return "KeyReleased: " + std::to_string(keyCode);
		}
		EventCategory getCategoryFlags() const override {
			return EventCategory::KeyEvent;
		}
	private:
		int keyCode;
	};
	class ENGINE_API EventDispatcher {//������÷��ͱ�̵��¼�������
	public:
		EventDispatcher(Event& event) :m_event(event) {}
		template<typename T>
		bool Dispatch(std::function<bool(T&)> func) {//��鷺�͵������Ƿ�ƥ�䣬�����Խ���ǿ������ת��
			if (T::getStaticType() == m_event::getStaticType()) {
				// �������ƥ�䣬����ô���ĺ���
				//return func(static_cast<T&>(m_event));
				m_event.m_handled = func(*(T*)&m_event);
				return m_event.m_handled; // �����¼��Ƿ񱻴���

			}
			return false;
		}
	protected:
		Event& m_event;
	};
}





