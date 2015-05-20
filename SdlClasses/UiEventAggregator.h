#ifndef UIEVENTAGGREGATOR
#define UIEVENTAGGREGATOR

#include <list>

class UiObserver;

/*	Add who will treat the event as observer.
	
	Set who will cause the event as publisher
	by inheriting from UiPublisher and calling
	it's UiPublisher::publishUiEvent method with the event 
	ids from here; or by passing lambdas referencing
	the UiEventAggregator instance and calling 
	UiEventAggregator::publishUiEvent 
	(using [&](int evtId){evtAggr.publishUiEvent(evtId);})
*/

class UiEventAggregator
{
	private:
		std::list<UiObserver*> observerList;

	public:
		void addUiEventObserver(UiObserver* obsv);
		void removeUiEventObserver(UiObserver* obsv);

		void publishUiEvent(int evtId);
};

#endif