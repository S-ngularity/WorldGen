#include "DefaultUiObjects/UiLabel.h"

#include "SdlClasses/SdlTexture.h"

#include <memory>

#include <iostream>
#include <string>

UiLabel::UiLabel(int xOff, int yOff, std::string text, int size, int r, int g, int b) : 
	UiLabel(xOff, yOff, ALIGN_TOP_LEFT, text, size, r, g, b)
{}

UiLabel::UiLabel(int xOff, int yOff, AlignMode alignMode, std::string text, int size, int r, int g, int b) : 
	UiObject(xOff, yOff, 0, 0), 
	labelFont(NULL), 
	align(alignMode), 
	labelText(text), 
	fontSize(size), 
	colorR(r), colorG(g), colorB(b), 
	originalX(xOff), originalY(yOff), alignedX(xOff), alignedY(yOff), 
	needUpdate(true)
{
	setPreRenderProcedure([&](){ labelPreRender(); });
	setPostRenderProcedure([&](){ labelPostRender(); });

	labelFont = TTF_OpenFont("Resources/OpenSans-Regular.ttf", fontSize);
	if(labelFont == NULL)
		std::cout << "Failed to load labelFont in UiLabel! SDL_ttf Error: " << TTF_GetError() << std::endl;
	else
		TTF_SetFontStyle(labelFont, TTF_STYLE_BOLD);
}

UiLabel::~UiLabel()
{
	if(labelFont != NULL)
		TTF_CloseFont(labelFont);
}

void UiLabel::setText(std::string text)
{
	labelText = text;
	needUpdate = true;
}

void UiLabel::setSize(int size)
{
	fontSize = size;
	needUpdate = true;
}

void UiLabel::setColor(int r, int g, int b)
{
	colorR = r;
	colorG = g;
	colorB = b;
	needUpdate = true;
}

void UiLabel::setAlignMode(AlignMode a)
{
	align = a;
}

void UiLabel::labelPreRender()
{
	if(needUpdate)
	{
		if(labelText.empty())
		{
			setUiObjectTexture(NULL);
			setUiObjectSize(0, 0);
		}

		else
		{
			if(parentUiManager == NULL)
			{
				std::cout << "UiLabel (\"" << labelText << "\") without parentUiManager (is NULL)." << std::endl;
				return;
			}

			if(labelFont == NULL)
			{
				std::cout << "Failed to load labelFont in UiLabel! SDL_ttf Error: " << TTF_GetError() << std::endl;
				return;
			}

			SDL_Surface* tempSurface = TTF_RenderText_Blended_Wrapped(labelFont, labelText.c_str(), {(Uint8) colorR, (Uint8) colorG, (Uint8) colorB}, 1920);
										//TTF_RenderText_Blended(labelFont, labelText.c_str(), {(Uint8) colorR, (Uint8) colorG, (Uint8) colorB});
			if(tempSurface == NULL)
			{
				std::cout << "Unable to render text surface in UiLabel! SDL_ttf Error: " << TTF_GetError() << std::endl;
				return;
			}

			//Create texture from surface pixels
			SDL_Texture *tempTex = SDL_CreateTextureFromSurface(parentUiManager->getRenderer(), tempSurface); // SDL_TEXTUREACCESS_STATIC
			if(tempTex == NULL)
			{
				std::cout << "Unable to create texture from rendered text in UiLabel! SDL Error: " << SDL_GetError() << std::endl;
				SDL_FreeSurface(tempSurface);

				return;
			}

			setUiObjectTexture(std::make_shared<SdlTexture>(tempTex, tempSurface->w, tempSurface->h));
			setUiObjectSize(tempSurface->w, tempSurface->h);
			setUiObjectLogicalSize(0, 0);

			SDL_FreeSurface(tempSurface);
		}

		getUiObjectOffset(&originalX, &originalY);

		if(align == ALIGN_TOP_LEFT || align == ALIGN_CENTER_LEFT || align == ALIGN_BOTTOM_LEFT)
			alignedX = originalX;

		else if(align == ALIGN_TOP_CENTER || align == ALIGN_CENTER_CENTER || align == ALIGN_BOTTOM_CENTER)
			alignedX = originalX - getWidth() / 2.f;

		else if(align == ALIGN_TOP_RIGHT || align == ALIGN_CENTER_RIGHT || align == ALIGN_BOTTOM_RIGHT)
			alignedX = originalX - getWidth();


		if(align == ALIGN_TOP_LEFT || align == ALIGN_TOP_CENTER || align == ALIGN_TOP_RIGHT)
			alignedY = originalY;

		else if(align == ALIGN_CENTER_LEFT || align == ALIGN_CENTER_CENTER || align == ALIGN_CENTER_RIGHT)
			alignedY = originalY - getHeight() / 2.f;

		else if(align == ALIGN_BOTTOM_LEFT || align == ALIGN_BOTTOM_CENTER || align == ALIGN_BOTTOM_RIGHT)
			alignedY = originalY - getHeight();

		needUpdate = false;
	}
	
	setUiObjectOffset(alignedX, alignedY);
}

void UiLabel::labelPostRender()
{
	setUiObjectOffset(originalX, originalY);
}
