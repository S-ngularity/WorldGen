#include "DefaultUiObjects/UiButton.h"

#include "SdlClasses/SdlTexture.h"

UiButton::UiButton(int xOff, int yOff, SdlTexture *t, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{}

UiButton::UiButton(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<void()> btEvtH) : 
	UiObject(xOff, yOff, w, h, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{}

bool UiButton::buttonEvtHandler(SDL_Event &e)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		if(customButtonAction)
			customButtonAction();

		return true;
	}

	else
		return false;
}