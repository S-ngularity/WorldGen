#include "SdlClasses/SdlWindows/NoiseWindow.h"

#include "Map.h"
#include "Noises/MyNoise.h"
#include "Noises/DiamSqNoise.h"
#include "Noises/OpenSimplexNoise.h"
#include "SdlClasses/UiObject.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

NoiseWindow::NoiseWindow(Map* mapVect[], int num) : 
	SdlWindow("WorldGen", 20, 40, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE), // superclass window constructor
	mapTexture(mapVect[0], getRenderer()),
	walkWindow(mapVect[0])
{
	setEventHandler([&](SDL_Event &e){eventHandlerFunc(e);});
	
	this->mapVect = mapVect;
	numMaps = num;
	selectedMap = 0;

	noiseVect[0] = new OpenSimplexNoise(mapVect[0], octaves, freq, persistence, freqDiv);
	noiseVect[1] = new DiamSqNoise(mapVect[0]);
	noiseVect[2] = new MyNoise(mapVect[0]);
	selectedNoise = 0;

	heightInfoFont = TTF_OpenFont("Resources/OpenSans-Regular.ttf", 20);
	if(heightInfoFont == NULL)
		std::cout << "Failed to load heightInfoFont! SDL_ttf Error: " << TTF_GetError() << std::endl;

	else
		TTF_SetFontStyle(heightInfoFont, TTF_STYLE_BOLD);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	//SDL_RenderSetLogicalSize(getRenderer(), mapVect[selectedMap]->getMapWidth(), mapVect[selectedMap]->getMapHeight());

	createGui();

	//Initialize renderer color
	SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);

	mapTexture.update();
	mapTexture.renderFitToArea(getRenderer(), 0, 0, getWindowWidth(), getWindowHeight());
	gui->renderScaled(getRenderer(), 0, 0, getWindowWidthScale(), getWindowHeightScale());
	refresh();
}

NoiseWindow::~NoiseWindow()
{
	for(int i = 0; i < 3; i++)
		delete noiseVect[i];

	TTF_CloseFont(heightInfoFont);
}

SDL_Texture* NoiseWindow::createDrawnTexture(int width, int height, int r, int g, int b, int a)
{
	SDL_Texture *targetTex = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetRenderTarget(getRenderer(), targetTex);
	SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
	SDL_RenderClear(getRenderer());
	SDL_SetRenderTarget(getRenderer(), NULL);

	return targetTex;
}

void NoiseWindow::createGui()
{
	gui = new UiObject(0, 0, NULL, getWindowWidth(), getWindowHeight(), NULL);

	SDL_Texture *btMap1Tex = createDrawnTexture(30, 30, 0, 255, 0, 255);
	gui->addChild(new UiObject(	30, gui->getHeight() - 60, 
								btMap1Tex, 30, 30, 
								[&](SDL_Event &e){return btMapClicked(e, 0);}));

	SDL_Texture *btMap2Tex = createDrawnTexture(30, 30, 0, 255, 0, 255);
	gui->addChild(new UiObject(	80, gui->getHeight() - 60, 
								btMap2Tex, 30, 30, 
								[&](SDL_Event &e){return btMapClicked(e, 1);}));

	SDL_Texture *btMap3Tex = createDrawnTexture(30, 30, 0, 255, 0, 255);
	gui->addChild(new UiObject(130, gui->getHeight() - 60, 
								btMap3Tex, 30, 30, 
								[&](SDL_Event &e){return btMapClicked(e, 2);}));

	SDL_Texture *btNoise1Tex = createDrawnTexture(30, 30, 0, 255, 0, 255);
	gui->addChild(new UiObject(gui->getWidth() - 160, gui->getHeight() - 60, 
								btNoise1Tex, 30, 30, 
								[&](SDL_Event &e){return btNoiseClicked(e, 0);}));

	SDL_Texture *btNoise2Tex = createDrawnTexture(30, 30, 0, 255, 0, 255);
	gui->addChild(new UiObject(gui->getWidth() - 110, gui->getHeight() - 60, 
								btNoise2Tex, 30, 30, 
								[&](SDL_Event &e){return btNoiseClicked(e, 1);}));

	SDL_Texture *btNoise3Tex = createDrawnTexture(30, 30, 0, 255, 0, 255);
	ui->addChild(new UiObject(gui->getWidth() - 60, gui->getHeight() - 60, 
								btNoise3Tex, 30, 30, 
								[&](SDL_Event &e){return btNoiseClicked(e, 2);}));
	
	/*// tests
	auto parent = new UiObject(getWindowWidth() - 60, getWindowHeight() - 60, btNoise3Tex, 30, 30, 
								[&](SDL_Event &e){	if(e.type == SDL_MOUSEBUTTONUP){
														cout << "parent button clicked" << endl;
														return true;
													}else if(e.type == SDL_KEYDOWN)
													{
														if(e.key.keysym.sym == SDLK_p)
														{
															cout << "parent keyboard pressed" << endl;
															return true;
														}
													}
													return false;});

	SDL_Texture *childTex = createDrawnTexture(10, 10, 0, 0, 255, 255);
	auto childObj = new UiObject(10, 10, childTex, 10, 10, 
								[&](SDL_Event &e){	if(e.type == SDL_MOUSEBUTTONUP){
														cout << "child button clicked" << endl;
														return true;
													}else if(e.type == SDL_KEYDOWN)
													{
														if(e.key.keysym.sym == SDLK_p)
														{
															cout << "child keyboard pressed" << endl;
															return true;
														}
													}
													return false;});
	parent->addChild(childObj);
	gui->addChild(parent);//*/
}

bool updateMapEvent = false;

bool NoiseWindow::btMapClicked(SDL_Event &e, int i)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		selectedMap = i;
		mapTexture.setMap(mapVect[selectedMap]);
		cout << endl << endl << "Map " << i+1 << endl;
		cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
		updateMapEvent = true;

		return true;
	}

	else
		return false;
}

bool NoiseWindow::btNoiseClicked(SDL_Event &e, int i)
{
	if(e.type == SDL_MOUSEBUTTONUP)
	{
		selectedNoise = i;

		return true;
	}

	else
		return false;
}

void NoiseWindow::eventHandlerFunc(SDL_Event& e)
{
	walkWindow.handleEvent(e);

	if(hasKeyboardFocus())
	{
		gui->handleEvent(e);

		bool updateMapTexture = updateMapEvent;
		bool shouldRender = false;
		updateMapEvent = false;

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
					walkWindow.show(); // must happen before setPos
					walkWindow.setPos(x, y);
				}
			break;

			case SDL_MOUSEMOTION:
				updateInfoTex();
				shouldRender = true;
			break;

			/*
			case SDL_WINDOWEVENT:
				if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					shouldRender = true;
			break;//*/
		}

		if(updateMapTexture)
		{
			updateInfoTex(); // in case it became sea
			mapTexture.update();

			shouldRender = true;
		}

		if(shouldRender)
		{
			//Clear screen
			SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);
			SDL_RenderClear(getRenderer());
			
			mapTexture.renderFitToArea(getRenderer(), 0, 0, getWindowWidth(), getWindowHeight());
			
			int x, y;
			//mapPosFromMouse(&x, &y); // usar se window tiver logical size ativado
			SDL_GetMouseState(&x, &y);
			heightInfoTex.render(getRenderer(), x, y - 30); // render tamanho original da fonte
			
			gui->renderScaled(getRenderer(), 0, 0, getWindowWidthScale(), getWindowHeightScale());
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
			mapTexture.renderFitToArea(getRenderer(), 0, 0, getWindowWidth(), getWindowHeight());
			
			refresh();

			updateMapTexture = false;
		}

		SDL_PumpEvents();
	}

	mapVect[selectedMap]->setSeaLvl(SEA_LEVEL);
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
		std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
		return;
	}

	else
	{
		//Create texture from surface pixels
		SDL_Texture *tempTex = SDL_CreateTextureFromSurface(getRenderer(), tempSurface); // SDL_TEXTUREACCESS_STATIC
		if(tempTex == NULL)
		{
			std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
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

	int winW = getWindowWidth(), winH = getWindowHeight();
	//SDL_GetWindowSize(window, &winW, &winH);

	//float scaleX, scaleY;
	//SDL_RenderGetScale(getRenderer(), &scaleX, &scaleY);
	double scaleX, scaleY;
	scaleX = (double)getWindowWidth() / mapVect[selectedMap]->getMapWidth();
	scaleY = (double)getWindowHeight() / mapVect[selectedMap]->getMapHeight();

	int sobra; // letterbox, soma dos 2 lados
	
	// regra de três: mouseX / winW assim como mapX / mapVect[selectedMap]->getMapWidth()
	// if compara ratioW - ratioH (ratio do original em relação a janela) para saber 
	// se letterbox está dos lados ou em cima/baixo
	if((double)mapVect[selectedMap]->getMapWidth() / winW - (double)mapVect[selectedMap]->getMapHeight() / winH > 0)
	{
		// total - tamanho do mapa(renderer logical size) * escala que esta sendo renderizado
		sobra = (winH - mapVect[selectedMap]->getMapHeight() * scaleY);
		
		*x = (*x * mapVect[selectedMap]->getMapWidth())/(double)winW;
		if(*y < sobra/2) // está no letterbox antes
			*y = 0;

		else if(*y > mapVect[selectedMap]->getMapHeight() * scaleY + sobra/2) // está no letterbox depois
			*y = mapVect[selectedMap]->getMapHeight() - 1;

		else // escala y sem letterbox em window size sem letterbox em relação ao mapa
			*y = ((*y - sobra/2) * mapVect[selectedMap]->getMapHeight())/(double)(winH - sobra);
	}

	else
	{
		sobra = (winW - mapVect[selectedMap]->getMapWidth() * scaleX);

		*y = (*y * mapVect[selectedMap]->getMapHeight())/(double)winH;

		if(*x < sobra/2)
			*x = 0;

		else if(*x >  mapVect[selectedMap]->getMapWidth() * scaleX + sobra/2)
			*x = mapVect[selectedMap]->getMapWidth() - 1;

		else
			*x = ((*x - sobra/2) * mapVect[selectedMap]->getMapWidth())/(double)(winW - sobra);
	}
}
