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

		bool buttonEvtHandler(const SDL_Event &e);

		void buttonPreRender();

	public:
		UiButton(int xOff, int yOff, 						// Position (offset)
					std::shared_ptr<UiLabel> textLabel, 	// Text label
					std::shared_ptr<SdlTexture> t, 			// Three part texture (standard/hover/pressed, vertically aligned)
					std::function<void()> btAction);		// Event handler

		UiButton(int xOff, int yOff, 						//
					int w, int h, 							// Size
					std::shared_ptr<UiLabel> textLabel,		//
					std::shared_ptr<SdlTexture> t, 			//
					std::function<void()> btAction);		//
};

# endif
