#ifndef UIEVENTAGGREGATOR
#define UIEVENTAGGREGATOR

#include <list>

class UiObserver;

class UiEventAggregator
{
	private:
		static UiEventAggregator *evtAggrInstance;

		UiEventAggregator(){} // private so that it can't be called
		UiEventAggregator(UiEventAggregator const&) = delete;
		UiEventAggregator& operator=(UiEventAggregator const&) = delete;

		static UiEventAggregator* Instance() { if (evtAggrInstance == nullptr) evtAggrInstance = new UiEventAggregator; return evtAggrInstance;}

		std::list<UiObserver*> observerList;

		void addUiEventObserver(UiObserver* obsv);
		void removeUiEventObserver(UiObserver* obsv);

		void publishUiEvent(int evtId);

		friend class UiPublisher;
		friend class UiObserver;
};

#endif