#include "SdlClasses/SdlWindows/NoiseWindow.h"

#include <stdio.h>

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

NoiseWindow::NoiseWindow(Map* mapVect[], int num) : 
	SdlWindow("WorldGen", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED), 
	mapTexture(mapVect[0], getRenderer()),
	walkWindow(mapVect[0])
{
	this->mapVect = mapVect;
	numMaps = num;
	selectedMap = 0;

	noiseVect = new Noise*[3];
	noiseVect[0] = new OpenSimplexNoise(mapVect[0], octaves, freq, persistence, freqDiv);
	noiseVect[1] = new DiamSqNoise(mapVect[0]);
	noiseVect[2] = new MyNoise(mapVect[0]);
	selectedNoise = 0;

	heightInfoFont = TTF_OpenFont("Resources/OpenSans-Regular.ttf", 20);
	if(heightInfoFont == NULL)
		printf("Failed to load heightInfoFont! SDL_ttf Error: %s\n", TTF_GetError());

	else
		TTF_SetFontStyle(heightInfoFont, TTF_STYLE_BOLD);

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(getRenderer(), mapVect[selectedMap]->getMapWidth(), mapVect[selectedMap]->getMapHeight());

	//Initialize renderer color
	SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);

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
						else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapVect[selectedMap]->getSeaLvl() + 1 < mapVect[selectedMap]->getHighestH())
						{
							mapVect[selectedMap]->increaseSeaLvl();
							cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
							
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
						else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapVect[selectedMap]->getSeaLvl() - 1 > 0 && mapTexture.getSeaRenderMode() == WITH_SEA)
						{
							mapVect[selectedMap]->decreaseSeaLvl();
							cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();

							updateMapTexture = true;
						}
					break;

					case SDLK_z:
						if(mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setLandRenderMode(VARYING_HIGHEST);
							updateMapTexture = true;
						}
					break;

					case SDLK_x:
						if(mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setLandRenderMode(VARYING_MAX);
							updateMapTexture = true;
						}
					break;

					case SDLK_c:
						if(mapTexture.getSeaRenderMode() != NO_SEA)
						{
							mapTexture.setLandRenderMode(FIXED);
							updateMapTexture = true;
						}
					break;

					case SDLK_r:
						resetNoise();
						runNoise();
					break;

					case SDLK_n:
						int n;
						cout << endl << "Normalize: ";
						cin >> n;
						mapVect[selectedMap]->normalize(n);
						mapVect[selectedMap]->setSeaLvl((mapVect[selectedMap]->getHighestH() / 2 ) - 1);
						cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
						updateMapTexture = true;
					break;

					case SDLK_1:
						if(e.key.keysym.mod & KMOD_SHIFT)
							selectedNoise = 0;

						else if(!(e.key.keysym.mod & KMOD_SHIFT))
						{
							selectedMap = 0;
							mapTexture.setMap(mapVect[selectedMap]);
							cout << endl << endl << "Map 1" << endl;
							cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
							updateMapTexture = true;
						}
						
					break;

					case SDLK_2:
						if(e.key.keysym.mod & KMOD_SHIFT)
							selectedNoise = 1;

						else if(!(e.key.keysym.mod & KMOD_SHIFT))
						{
							selectedMap = 1;
							mapTexture.setMap(mapVect[selectedMap]);
							cout << endl << endl << "Map 2" << endl;
							cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
							updateMapTexture = true;
						}
					break;

					case SDLK_3:
						if(e.key.keysym.mod & KMOD_SHIFT)
							selectedNoise = 2;

						else if(!(e.key.keysym.mod & KMOD_SHIFT))
						{
							selectedMap = 2;
							mapTexture.setMap(mapVect[selectedMap]);
							cout << endl << endl << "Map 3" << endl;
							cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
							updateMapTexture = true;
						}
					break;
				}
			break;

			case SDL_MOUSEBUTTONUP:
				if(e.button.button == SDL_BUTTON_RIGHT)
				{
					int x, y;
					mapPosFromMouse(&x, &y);

					walkWindow.setMap(mapVect[selectedMap]);
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
			SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);
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
	int shownPercent = -1;

	noiseVect[selectedNoise]->setMap(mapVect[selectedMap]);

	cout << endl << endl;

	while(noiseVect[selectedNoise]->getPercentComplete() < 100) // noise iterations
	{
		noiseVect[selectedNoise]->runOnce();

		// update only once per percent
		if(noiseVect[selectedNoise]->getPercentComplete() != shownPercent)
		{
			shownPercent = noiseVect[selectedNoise]->getPercentComplete();

			cout << "\b\b\b\b" << shownPercent << "%";

			if(shownPercent == 100) // show highest at each phase (if appliable)
				cout << endl 
				<< endl << "Highest point: " << mapVect[selectedMap]->getHighestH()
				<< endl << "Lowest point: " << mapVect[selectedMap]->getLowestH() << endl << endl;
			
			if(shownPercent % UPDATE_AT_PERCENT == 0)
			{
				if(noiseVect[selectedNoise]->getPercentComplete() < 100) // no sea while not done
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
					cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
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

		SDL_PumpEvents();
	}
}

void NoiseWindow::resetNoise()
{
	noiseVect[selectedNoise]->reset();
}

void NoiseWindow::updateInfoTex()
{
	int x, y;
	mapPosFromMouse(&x, &y);

	int h = mapVect[selectedMap]->Tile(x, y).getH();
	string info;

	if(mapTexture.getSeaRenderMode() == WITH_SEA && h <= mapVect[selectedMap]->getSeaLvl())
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

	// regra de três: mouseX / winW assim como mapX / mapVect[selectedMap]->getMapWidth()
	int sobra; // letterbox, soma dos 2 lados
	if((float)mapVect[selectedMap]->getMapWidth() / winW - (float)mapVect[selectedMap]->getMapHeight() / winH > 0) // ratioW - ratioH (ratio do original em relação a janela)
	{
		sobra = (winH - mapVect[selectedMap]->getMapHeight() * scaleY); // total - tamanho do mapa(renderer logical size) * escala que esta sendo renderizado
		
		*x = (*x * mapVect[selectedMap]->getMapWidth())/(float)winW;
		if(*y < sobra/2) // está no letterbox antes
			*y = 0;

		else if(*y > mapVect[selectedMap]->getMapHeight() * scaleY + sobra/2) // está no letterbox depois
			*y = mapVect[selectedMap]->getMapHeight() - 1;

		else // escala y sem letterbox em window size sem letterbox em relação ao mapa
			*y = ((*y - sobra/2) * mapVect[selectedMap]->getMapHeight())/(float)(winH - sobra);
	}

	else
	{
		sobra = (winW - mapVect[selectedMap]->getMapWidth() * scaleX);

		*y = (*y * mapVect[selectedMap]->getMapHeight())/(float)winH;

		if(*x < sobra/2)
			*x = 0;

		else if(*x >  mapVect[selectedMap]->getMapWidth() * scaleX + sobra/2)
			*x = mapVect[selectedMap]->getMapWidth() - 1;

		else
			*x = ((*x - sobra/2) * mapVect[selectedMap]->getMapWidth())/(float)(winW - sobra);
	}
}
