#ifndef UILABEL
#define UILABEL

#include "Ui/UiObject.h"

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class UiLabel : public UiObject
{
	private:
		TTF_Font *labelFont;
		std::string labelText;
		int fontSize;
		int colorR, colorG, colorB;

		bool needUpdate;

		void labelPreRender();

	public:
		UiLabel(int xOff, int yOff, std::string text, int size, int r, int g, int b);
		~UiLabel();

		void setText(std::string text);
		void setSize(int size);
		void setColor(int r, int g, int b);
};

# endif
