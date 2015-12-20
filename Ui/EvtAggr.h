#ifndef EVENTAGGREGATOR
#define EVENTAGGREGATOR

#include <list>

#include "Ui/Event.h"

class EvtAggr
{
	private:
		//static EvtAggr evtAggrInstance;

		EvtAggr(){} // private so that it can't be called
		EvtAggr(EvtAggr const&) = delete;
		EvtAggr& operator=(EvtAggr const&) = delete;

		template<typename EventType>
		static Event<EventType>& getEvent()
		{
			static Event<EventType> evt;

			return evt;
		}

	public:
		/*static EvtAggr& getEvent() {return evtAggrInstance;}

		template<typename EventType>
		Event<EventType>& getEvent()
		{
			static Event<EventType> evt;

			return evt;
		}*/

		/*template<typename EventType>
		static Event<EventType>& getEvent()
		{
			static Event<EventType> evt;

			return evt;
		}*/

		template<typename EventType>
		static void publish(EventType evt)
		{
			getEvent<EventType>().publish(evt);
		}

		template<typename EventType>
		static long subscribe(std::function<void(EventType &evt)> action)
		{
			return getEvent<EventType>().subscribe(action);
		}

		template<typename EventType>
		static void unsubscribe(long id)
		{
			getEvent<EventType>().unsubscribe(id);
		}
};

#endif
