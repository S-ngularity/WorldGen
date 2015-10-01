#ifndef UIPANEL
#define UIPANEL

#include "Ui/UiObject.h"

class UiPanel : public UiObject
{
	public:
		UiPanel(int xOff, int yOff, SdlTexture *t);
		UiPanel(int xOff, int yOff, int w, int h, SdlTexture *t);
		//virtual ~UiPanel();
};

# endif