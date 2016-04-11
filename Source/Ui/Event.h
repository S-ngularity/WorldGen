#ifndef EVENT
#define EVENT

#include <unordered_map>
#include <functional>
#include <iostream>
template<typename EventType>
class Event
{
	private:
		long nextKey;
		std::unordered_map< long, std::function<void(EventType &evt)> > subscriberMap;

	public:
		Event() : 
			nextKey(0)
		{}

		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;

		long subscribe(std::function<void(EventType &evt)> action)
		{
			subscriberMap[nextKey] = action;
			nextKey++;

			return nextKey - 1;
		}

		void unsubscribe(long id)
		{
			subscriberMap.erase(id);
		}

		void publish(EventType evt)
		{
			for(auto action : subscriberMap)
				action.second(evt);
		}
};

#endif