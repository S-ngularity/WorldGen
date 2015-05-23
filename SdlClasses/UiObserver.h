#ifndef UIOBSERVER
#define UIOBSERVER

#include "UiEventAggregator.h"

class UiObserver
{
	public:
		void addUiEventObserver(UiObserver* obsv)
		{
			UiEventAggregator::Instance()->addUiEventObserver(obsv);
		}
		
		void removeUiEventObserver(UiObserver* obsv)
		{
			UiEventAggregator::Instance()->removeUiEventObserver(obsv);
		}

		virtual bool handleUiEvent(int evtId) = 0;
};

#endif