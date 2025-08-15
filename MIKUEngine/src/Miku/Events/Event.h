#pragma once
#include"Miku/Core/CoreDefinitions.h"
#include "mikupch.h"
namespace MIKU {
	enum class EventType 
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled

	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType(){return EventType::##type;}\
										virtual EventType GetEventType() const  override{return GetStaticType();}\
										virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class MIKU_API Event {
		friend class EventDispatcher;
	public :
		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) {
			
			return GetCategoryFlags() & category;
			
		}
	protected:
		

	};
	
	class EventDispatcher {
		
		template<typename T>
		using EventFn = std::function<bool(T&)>;//f返回类型是bool，函数参数是T&
	public:
		EventDispatcher(Event& event) :m_Event(event) 
		{
		
		}
		template<typename T>
		//func是回调函数
		bool Dispatch(EventFn<T> func) 
		{
			if (m_Event.GetEventType() == T::GetStaticType())//如果传入的事件等于模板参数的特定事件，则进行消费
			{
				m_Event.Handled = func(*(T*)&m_Event);//取m_Event的地址转换成T*(子类事件),最后在解引用，当作参数传递给回调函数
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}

}



