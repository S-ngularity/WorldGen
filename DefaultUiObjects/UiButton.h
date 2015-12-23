#ifndef UIBUTTON
#define UIBUTTON

#include <functional>
#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include "Ui/UiObject.h"

class SdlTexture;
class UiLabel;

class UiButton : public UiObject
{
	private:
		std::function<void()> customButtonAction;
		std::shared_ptr<SDL_Rect> standardClipRect, hoverClipRect, pressedClipRect;
		UiLabel *labelPtr;
		
		bool clickHappenedHere;
		bool isPressed;

		bool buttonEvtHandler(SDL_Event &e);

		void buttonPreRender();

	public:
		UiButton(int xOff, int yOff, 						// Position (offset)
					std::shared_ptr<UiLabel> textLabel, 	// Text label
					std::shared_ptr<SdlTexture> t, 			// Three part texture (standard/hover/pressed, side by side)
					std::function<void()> btEvtH);			// Event handler

		UiButton(int xOff, int yOff, 						//
					int w, int h, 							// Size
					std::shared_ptr<UiLabel> textLabel,		//
					std::shared_ptr<SdlTexture> t, 			//
					std::function<void()> btEvtH);			//
};

# endif
