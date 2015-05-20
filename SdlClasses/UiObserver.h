#ifndef UIOBSERVER
#define UIOBSERVER

class UiObserver
{
	public:
		virtual bool handleUiEvent(int evtId) = 0;
};

#endif