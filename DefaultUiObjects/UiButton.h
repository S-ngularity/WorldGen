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
		std::shared_ptr<SdlTexture> btTexture, btTextureHover, btTexturePressed;
		UiLabel *label;
		
		bool clickHappenedHere;
		bool isPressed;

		bool buttonEvtHandler(SDL_Event &e);

		void buttonPreRender();

	public:
		UiButton(int xOff, int yOff, 						// Position (offset)
					UiLabel *textLabel, 					// Text label
					std::shared_ptr<SdlTexture> t, 			// Static texture
					std::function<void()> btEvtH);			// Event handler

		UiButton(int xOff, int yOff, 						//
					int w, int h, 							// Size
					UiLabel *textLabel, 					//
					std::shared_ptr<SdlTexture> t, 			//
					std::function<void()> btEvtH);			//

		UiButton(int xOff, int yOff, 						//
					UiLabel *textLabel, 					//
					std::shared_ptr<SdlTexture> t, 			//
					std::shared_ptr<SdlTexture> tHover, 	// Dynamic texture
					std::shared_ptr<SdlTexture> tPressed, 	//
					std::function<void()> btEvtH);			//

		UiButton(int xOff, int yOff, 						//
					int w, int h, 							// Size
					UiLabel *textLabel, 					//
					std::shared_ptr<SdlTexture> t, 			//
					std::shared_ptr<SdlTexture> tHover, 	// Dynamic texture
					std::shared_ptr<SdlTexture> tPressed, 	//
					std::function<void()> btEvtH);			//
};

# endif
