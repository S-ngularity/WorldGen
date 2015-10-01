#ifndef UIBUTTON
#define UIBUTTON

#include <functional>

#include <SDL2/SDL.h>

#include "Ui/UiObject.h"

class SdlTexture;

class UiButton : public UiObject
{
	private:
		std::function<void()> customButtonAction;

		bool buttonEvtHandler(SDL_Event &e);

	public:
		UiButton(int xOff, int yOff, SdlTexture *t, std::function<void()> btEvtH);
		UiButton(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<void()> btEvtH);
		//virtual ~UiButton();
};

# endif