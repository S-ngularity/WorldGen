#ifndef UIPUBLISHER
#define UIPUBLISHER

#include "SdlClasses/UiEventAggregator.h"

class UiPublisher
{
	public:
		void publishUiEvent(UiEventAggregator *evtAggr, int evtId)
		{
			evtAggr->publishUiEvent(evtId);
		}
};

#endif