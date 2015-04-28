#include "SdlClasses/SdlWindows/NoiseWindow.h"

#include <stdio.h>

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

NoiseWindow::NoiseWindow(Map &theMap) : 
	SdlWindow("WorldGen", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED), 
	worldMap(theMap),
	mapTexture(theMap, getRenderer()),
	noise(theMap, octaves, freq, persistence, freqDiv),
	walkWindow(theMap)
{
	heightInfoFont = TTF_OpenFont("Resources/OpenSans-Regular.ttf", 20);
	if(heightInfoFont == NULL)
		printf("Failed to load heightInfoFont! SDL_ttf Error: %s\n", TTF_GetError());

	else
		TTF_SetFontStyle(heightInfoFont, TTF_STYLE_BOLD);

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(getRenderer(), worldMap.getMapWidth(), worldMap.getMapHeight());

	//Initialize renderer color
	SDL_SetRenderDrawColor(getRenderer(), 255, 0, 255, 255);

	mapTexture.update();
	mapTexture.render(getRenderer(), 0, 0);
	refresh();
}

NoiseWindow::~NoiseWindow()
{
	TTF_CloseFont(heightInfoFont);
}

void NoiseWindow::handleImplementedEvents(SDL_Event& e)
{
	walkWindow.handleEvent(e);

	if(hasKeyboardFocus())
	{
		bool updateMapTexture = false, shouldRender = false;
		
		switch(e.type)
		{
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym)
				{
					case SDLK_UP:
						// up always shows sea (without increasing sealvl on first press)
						if(mapTexture.getSeaRenderMode() != WITH_SEA)
						{
							mapTexture.setSeaRenderMode(WITH_SEA);
							updateMapTexture = true;
						}

						// up = +1 sea_lvl when with_sea
						else if(!(e.key.keysym.mod & KMOD_SHIFT) && worldMap.getSeaLvl() + 1 < worldMap.getHighestH())
						{
							worldMap.increaseSeaLvl();
							cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << worldMap.getSeaLvl();
							
							updateMapTexture = true;
						}
					break;

					case SDLK_DOWN:
						// shift+down = no_sea (no update when not needed)
						if(e.key.keysym.mod & KMOD_SHIFT && mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setSeaRenderMode(NO_SEA);
							mapTexture.setLandRenderMode(FIXED);
							updateMapTexture = true;
						}

						// down = -1 sealvl when with_sea
						else if(!(e.key.keysym.mod & KMOD_SHIFT) && worldMap.getSeaLvl() - 1 > 0 && mapTexture.getSeaRenderMode() == WITH_SEA);
						{
							worldMap.decreaseSeaLvl();
							cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << worldMap.getSeaLvl();

							updateMapTexture = true;
						}
					break;

					case SDLK_f:
						if(mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setLandRenderMode(FIXED);
							updateMapTexture = true;
						}
					break;

					case SDLK_h:
						if(mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setLandRenderMode(VARYING_HIGHEST);
							updateMapTexture = true;
						}
					break;

					case SDLK_m:
						if(mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setLandRenderMode(VARYING_MAX);
							updateMapTexture = true;
						}
					break;

					case SDLK_r:
						runNoise();
					break;

					case SDLK_n:
						int n;
						cout << endl << "Normalize: ";
						cin >> n;
						worldMap.normalize(n);
						worldMap.setSeaLvl((worldMap.getHighestH() / 2 ) - 1);
						updateMapTexture = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONUP:
				if(e.button.button == SDL_BUTTON_RIGHT)
				{
					int x, y;
					mapPosFromMouse(&x, &y);

					walkWindow.show();
					walkWindow.setPos(x, y);
				}
			break;

			case SDL_MOUSEMOTION:
				updateInfoTex();
				shouldRender = true;
			break;
		}

		if(updateMapTexture)
		{
			updateInfoTex(); // in case it became sea
			mapTexture.update();

			shouldRender = true;
		}

		if(shouldRender)
		{
			int x, y;
			mapPosFromMouse(&x, &y);

			//Clear screen
			SDL_SetRenderDrawColor(getRenderer(), 255, 0, 255, 255);
			SDL_RenderClear(getRenderer());
			
			mapTexture.render(getRenderer(), 0, 0);
			heightInfoTex.render(getRenderer(), x, y - 30);

			refresh();
		}
	}
}

void NoiseWindow::runNoise()
{
	bool updateMapTexture = false;
	int shownPercent = 0;

	while(noise.getPercentComplete() < 100) // noise iterations
	{
		noise.runOnce();

		// update only once per percent
		if(noise.getPercentComplete() != shownPercent)
		{
			shownPercent = noise.getPercentComplete();

			cout << "\b\b\b\b" << shownPercent << "%";

			if(shownPercent == 100) // show highest at each phase (if appliable)
				cout << endl 
				<< endl << "Highest point: " << worldMap.getHighestH()
				<< endl << "Lowest point: " << worldMap.getLowestH() << endl << endl;
			
			if(shownPercent % UPDATE_AT_PERCENT == 0)
			{
				if(noise.getPercentComplete() < 100) // no sea while not done
				{
					mapTexture.setSeaRenderMode(NO_SEA);
					mapTexture.setLandRenderMode(FIXED);
					updateMapTexture = true;
				}

				else
				{
					mapTexture.setSeaRenderMode(WITH_SEA);
					mapTexture.setLandRenderMode(VARYING_HIGHEST);
					updateMapTexture = true; // last noise print
					cout << "Sea Level : " << setw(3) << setfill('0') << worldMap.getSeaLvl();
				}
			}
		}

		if(updateMapTexture)
		{
			mapTexture.update();
			mapTexture.render(getRenderer(), 0, 0);
			
			refresh();

			updateMapTexture = false;
		}
	}
}

void NoiseWindow::updateInfoTex()
{
	int x, y;
	mapPosFromMouse(&x, &y);

	int h = worldMap.Tile(x, y).getH();
	string info;

	if(mapTexture.getSeaRenderMode() == WITH_SEA && h <= worldMap.getSeaLvl())
		info = "Sea";

	else
	{
		stringstream ss;
		ss << h;
		info = ss.str();
	}

	SDL_Surface* tempSurface = TTF_RenderText_Blended(heightInfoFont, info.c_str(), {220,20,60});
	if(tempSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return;
	}

	else
	{
		//Create texture from surface pixels
		SDL_Texture *tempTex = SDL_CreateTextureFromSurface(getRenderer(), tempSurface); // SDL_TEXTUREACCESS_STATIC
		if(tempTex == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			SDL_FreeSurface(tempSurface);

			return;
		}

		heightInfoTex.setTexture(tempTex, tempSurface->w, tempSurface->h);

		SDL_FreeSurface(tempSurface);
    }
}

void NoiseWindow::mapPosFromMouse(int *x, int *y)
{
	SDL_GetMouseState(x, y);

	int winW, winH;
	SDL_GetWindowSize(window, &winW, &winH);

	float scaleX, scaleY;
	SDL_RenderGetScale(getRenderer(), &scaleX, &scaleY);

	// regra de três: mouseX / winW assim como mapX / worldMap.getMapWidth()
	int sobra; // letterbox, soma dos 2 lados
	if((float)worldMap.getMapWidth() / winW - (float)worldMap.getMapHeight() / winH > 0) // ratioW - ratioH (ratio do original em relação a janela)
	{
		sobra = (winH - worldMap.getMapHeight() * scaleY); // total - tamanho do mapa(renderer logical size) * escala que esta sendo renderizado
		
		*x = (*x * worldMap.getMapWidth())/(float)winW;
		if(*y < sobra/2) // está no letterbox antes
			*y = 0;

		else if(*y > worldMap.getMapHeight() * scaleY + sobra/2) // está no letterbox depois
			*y = worldMap.getMapHeight() - 1;

		else // escala y sem letterbox em window size sem letterbox em relação ao mapa
			*y = ((*y - sobra/2) * worldMap.getMapHeight())/(float)(winH - sobra);
	}

	else
	{
		sobra = (winW - worldMap.getMapWidth() * scaleX);

		*y = (*y * worldMap.getMapHeight())/(float)winH;

		if(*x < sobra/2)
			*x = 0;

		else if(*x >  worldMap.getMapWidth() * scaleX + sobra/2)
			*x = worldMap.getMapWidth() - 1;

		else
			*x = ((*x - sobra/2) * worldMap.getMapWidth())/(float)(winW - sobra);
	}
}
