#ifndef UIPANEL
#define UIPANEL

#include "Ui/UiObject.h"

class UiPanel : public UiObject
{
	public:
		UiPanel(int xOff, int yOff, int width, int height, std::shared_ptr<SdlTexture> texture);
};

# endif