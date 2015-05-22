#ifndef UIEVENTAGGREGATOR
#define UIEVENTAGGREGATOR

#include <list>

class UiObserver;

class UiEventAggregator
{
	private:
		std::list<UiObserver*> observerList;

		void addUiEventObserver(UiObserver* obsv);
		void removeUiEventObserver(UiObserver* obsv);

		void publishUiEvent(int evtId);

		friend class UiPublisher;
		friend class UiObserver;
};

#endif