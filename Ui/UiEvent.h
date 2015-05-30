#ifndef UIEVENT
#define UIEVENT

#include <list>
#include <functional>
#include <iostream>
template<typename EventType>
class UiEvent
{
	private:
		std::list< std::function<void(EventType &evt)> > subscriberList;

	public:
		void subscribe(std::function<void(EventType &evt)> action)
		{
			subscriberList.push_front(action);
		}

		void unsubscribe(std::function<void(EventType &evt)> action)
		{
			//subscriberList.remove(action);
		}

		void publishUiEvent(EventType evt)
		{
			for(auto action : subscriberList)
				action(evt);
		}
};

#endif