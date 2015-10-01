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
		SdlTexture *btTexture, *btTextureHover, *btTexturePressed;
		
		bool isPressed;

		bool buttonEvtHandler(SDL_Event &e);

		void buttonPreRender();

	public:
		UiButton(int xOff, int yOff, SdlTexture *t, std::function<void()> btEvtH);
		UiButton(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<void()> btEvtH);
		UiButton(int xOff, int yOff, SdlTexture *t, SdlTexture *tHover, SdlTexture *tPressed, std::function<void()> btEvtH);
		UiButton(int xOff, int yOff, int w, int h, SdlTexture *t, SdlTexture *tHover, SdlTexture *tPressed, std::function<void()> btEvtH);
		//virtual ~UiButton();
};

# endif