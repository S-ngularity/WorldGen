#ifndef MOUSEHEIGHTTEXT
#define MOUSEHEIGHTTEXT

#include "SdlClasses/SdlTexture.h"

#include "Ui/UiObject.h"

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class MouseHeightText : public UiObject
{
	private:
		TTF_Font *heightTextFont;

	public:
		MouseHeightText(SDL_Renderer *r);
		~MouseHeightText();

		void update(std::string text);
};

#endif