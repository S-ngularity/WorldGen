#ifndef UIPANEL
#define UIPANEL

#include "Ui/UiObject.h"

class UiPanel : public UiObject
{
	public:
		UiPanel(int xOff, int yOff, std::shared_ptr<SdlTexture> t);
		UiPanel(int xOff, int yOff, int w, int h, std::shared_ptr<SdlTexture> t);
};

# endif