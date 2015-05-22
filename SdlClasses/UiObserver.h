#ifndef UIOBSERVER
#define UIOBSERVER

#include "UiEventAggregator.h"

class UiObserver
{
	public:
		void addUiEventObserver(UiEventAggregator *evtAggr, UiObserver* obsv)
		{
			evtAggr->addUiEventObserver(obsv);
		}
		
		void removeUiEventObserver(UiEventAggregator *evtAggr, UiObserver* obsv)
		{
			evtAggr->removeUiEventObserver(obsv);
		}

		virtual bool handleUiEvent(int evtId) = 0;
};

#endif