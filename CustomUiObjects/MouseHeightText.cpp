#include "CustomUiObjects/MouseHeightText.h"

#include <iostream>
#include <string>

MouseHeightText::MouseHeightText(UiManager *parentUiMngr) : 
	UiObject(0, 0, 0, 0)
{
	parentUiManager = parentUiMngr;
	
	heightTextFont = TTF_OpenFont("Resources/OpenSans-Regular.ttf", 20);
	if(heightTextFont == NULL)
		std::cout << "Failed to load heightTextFont in MouseHeightText! SDL_ttf Error: " << TTF_GetError() << std::endl;

	else
		TTF_SetFontStyle(heightTextFont, TTF_STYLE_BOLD);
}

MouseHeightText::~MouseHeightText()
{
	TTF_CloseFont(heightTextFont);
}

void MouseHeightText::update(std::string text)
{
	SDL_Surface* tempSurface = TTF_RenderText_Blended(heightTextFont, text.c_str(), {220,20,60});
	if(tempSurface == NULL)
	{
		std::cout << "Unable to render text surface in MouseHeightText! SDL_ttf Error: " << TTF_GetError() << std::endl;
		return;
	}

	else
	{
		if(parentUiManager == NULL)
			std::cout << "MouseHeightText without parentUiManager (is NULL)." << std::endl;

		//Create texture from surface pixels
		SDL_Texture *tempTex = SDL_CreateTextureFromSurface(parentUiManager->getRenderer(), tempSurface); // SDL_TEXTUREACCESS_STATIC
		if(tempTex == NULL)
		{
			std::cout << "Unable to create texture from rendered text in MouseHeightText! SDL Error: " << SDL_GetError() << std::endl;
			SDL_FreeSurface(tempSurface);

			return;
		}

		setUiObjectTexture(new SdlTexture(tempTex, tempSurface->w, tempSurface->h));
		setUiObjectSize(tempSurface->w, tempSurface->h);

		SDL_FreeSurface(tempSurface);
    }
}