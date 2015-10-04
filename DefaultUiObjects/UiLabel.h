#ifndef UILABEL
#define UILABEL

#include "Ui/UiObject.h"

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum AlignMode {ALIGN_TOP_LEFT, ALIGN_CENTER_LEFT, ALIGN_BOTTOM_LEFT, 
				ALIGN_TOP_CENTER, ALIGN_CENTER_CENTER, ALIGN_BOTTOM_CENTER,
				ALIGN_TOP_RIGHT, ALIGN_CENTER_RIGHT, ALIGN_BOTTOM_RIGHT};

class UiLabel : public UiObject
{
	private:
		TTF_Font *labelFont;
		int align;
		std::string labelText;
		int fontSize;
		int colorR, colorG, colorB;

		int originalX, originalY;
		int alignedX, alignedY;

		bool needUpdate;

		void labelPreRender();
		void labelPostRender();

	public:
		UiLabel(int xOff, int yOff, std::string text, int size, int r, int g, int b);
		UiLabel(int xOff, int yOff, AlignMode alignMode, std::string text, int size, int r, int g, int b);
		~UiLabel();

		void setText(std::string text);
		void setSize(int size);
		void setColor(int r, int g, int b);
		void setAlignMode(AlignMode a);
};

# endif
