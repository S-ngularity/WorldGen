#ifndef UIEVENTAGGREGATOR
#define UIEVENTAGGREGATOR

#include <list>

#include "Ui/UiEvent.h"

class UiEventAggregator
{
	private:
		static UiEventAggregator *evtAggrInstance;

		UiEventAggregator(){evtAggrInstance = nullptr;} // private so that it can't be called
		UiEventAggregator(UiEventAggregator const&) = delete;
		UiEventAggregator& operator=(UiEventAggregator const&) = delete;

	public:
		static UiEventAggregator* Instance() { if (evtAggrInstance == nullptr) evtAggrInstance = new UiEventAggregator; return evtAggrInstance;}

		template<typename EventType>
		UiEvent<EventType>& getEvent()
		{
			static UiEvent<EventType> evt;

			return evt;
		}
};

#endif