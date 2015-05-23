#ifndef UIPUBLISHER
#define UIPUBLISHER

#include "SdlClasses/UiEventAggregator.h"

class UiPublisher
{
	public:
		void publishUiEvent(int evtId)
		{
			UiEventAggregator::Instance()->publishUiEvent(evtId);
		}
};

#endif