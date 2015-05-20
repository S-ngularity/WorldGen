#ifndef UIPUBLISHER
#define UIPUBLISHER

#include "SdlClasses/UiEventAggregator.h"

class UiPublisher
{
	protected:
		UiEventAggregator *evtAggregator;

	public:
		UiPublisher(UiEventAggregator *evtAggr) {evtAggregator = evtAggr;}

		void publishUiEvent(int evtId) { evtAggregator->publishUiEvent(evtId); }
};

#endif