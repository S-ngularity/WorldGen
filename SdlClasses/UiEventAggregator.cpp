#include "SdlClasses/UiEventAggregator.h"

#include "SdlClasses/UiObserver.h"

void UiEventAggregator::addUiEventObserver(UiObserver* obsv)
{
	observerList.push_front(obsv);
}

void UiEventAggregator::removeUiEventObserver(UiObserver* obsv)
{
	observerList.remove(obsv);
}

void UiEventAggregator::publishUiEvent(int evtId)
{
	for(auto obsv : observerList)
		obsv->handleUiEvent(evtId);
}
