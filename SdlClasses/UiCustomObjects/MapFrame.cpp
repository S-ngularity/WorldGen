#include "SdlClasses/UiCustomObjects/MapFrame.h"

#include "Map.h"
#include "Noises/MyNoise.h"
#include "Noises/DiamSqNoise.h"
#include "Noises/OpenSimplexNoise.h"

#include "SdlClasses/SdlTextures/MapTexture.h"

#include "SdlClasses/UiEventCodes.h"

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

using namespace std;

MapFrame::MapFrame(SDL_Renderer *r, int x, int y, int w, int h, Map* mapVect[], int num) : 
	UiObject(r, x, y, w, h, NULL,
			[&](SDL_Event &e){return handleInternalSdlEvent(e);})
{
	// make mapFrame observe events, so it may respond to other UiObjects
	addUiEventObserver(this);

	this->mapVect = mapVect;
	numMaps = num;
	selectedMap = 0;

	noiseVect[0] = new OpenSimplexNoise(mapVect[0], octaves, freq, persistence, freqDiv);
	noiseVect[1] = new DiamSqNoise(mapVect[0]);
	noiseVect[2] = new MyNoise(mapVect[0]);
	selectedNoise = 0;

	mapTexture = new MapTexture(getRenderer(), mapVect[selectedMap]);
	mapTexture->update();
	setUiObjectTexture(mapTexture);

	mouseText = new MouseHeightText(getRenderer());
	setPostRenderProcedure([&](){ renderMouseText(); });
	
	publishUiEvent(UIEVT_CONTENTSCHANGED);
}

MapFrame::~MapFrame()
{
	for(int i = 0; i < 3; i++)
		delete noiseVect[i];

	removeUiEventObserver(this);
}

void MapFrame::runNoise()
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
					mapTexture->setSeaRenderMode(NO_SEA);
					mapTexture->setLandRenderMode(FIXED);
					updateMapTexture = true;
				}

				else
				{
					mapTexture->setSeaRenderMode(WITH_SEA);
					mapTexture->setLandRenderMode(VARYING_HIGHEST);
					updateMapTexture = true; // last noise print
					cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
				}
			}
		}

		if(updateMapTexture)
		{
			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
			updateMapTexture = false;
		}

		SDL_PumpEvents();
	}

	mapVect[selectedMap]->setSeaLvl(SEA_LEVEL);
}

void MapFrame::resetNoise()
{
	noiseVect[selectedNoise]->reset();
}

void MapFrame::updateMouseText()
{
	int x, y;
	
	if(mapPosFromMouse(&x, &y))
	{
		int h = mapVect[selectedMap]->Tile(x, y).getH();
		string text;

		if(mapTexture->getSeaRenderMode() == WITH_SEA && h <= mapVect[selectedMap]->getSeaLvl())
			text = "Sea";

		else
		{
			stringstream ss;
			ss << h;
			text = ss.str();
		}

		mouseText->update(text);
	}
}

void MapFrame::renderMouseText()
{
	mouseText->render(getAbsoluteX(), getAbsoluteY());
}

bool MapFrame::mapPosFromMouse(int *x, int *y)
{
	if(!UiObject::getRelativeMousePos(this, x, y))
		return false;

	int tamTexW = getWidth(), tamTexH = getHeight();

	double scaleX, scaleY;
	scaleX = (double)tamTexW / mapVect[selectedMap]->getMapWidth();
	scaleY = (double)tamTexH / mapVect[selectedMap]->getMapHeight();

	int sobra; // letterbox, soma dos 2 lados
	
	// regra de três: mouseX / winW assim como mapX / mapVect[selectedMap]->getMapWidth()
	// regra de três: mouseX / tamanhoTex assim como mapX / mapVect[selectedMap]->getMapWidth()
	// if compara ratioW - ratioH (ratio do original em relação a janela) para saber 
	// se letterbox está dos lados ou em cima/baixo
	if((double)mapVect[selectedMap]->getMapWidth() / tamTexW - (double)mapVect[selectedMap]->getMapHeight() / tamTexH > 0)
	{
		// total - tamanho do mapa(renderer logical size) * escala que esta sendo renderizado
		sobra = (tamTexH - mapVect[selectedMap]->getMapHeight() * scaleY);
		
		*x = (*x * mapVect[selectedMap]->getMapWidth()) / (double)tamTexW;
		if(*y < sobra/2) // está no letterbox antes
			*y = 0;

		else if(*y > mapVect[selectedMap]->getMapHeight() * scaleY + sobra/2) // está no letterbox depois
			*y = mapVect[selectedMap]->getMapHeight() - 1;

		else // escala y sem letterbox em window size sem letterbox em relação ao mapa
			*y = ((*y - sobra/2) * mapVect[selectedMap]->getMapHeight()) / (double)(tamTexH - sobra);
	}

	else
	{
		sobra = (tamTexW - mapVect[selectedMap]->getMapWidth() * scaleX);

		*y = (*y * mapVect[selectedMap]->getMapHeight()) / (double)tamTexH;

		if(*x < sobra/2)
			*x = 0;

		else if(*x >  mapVect[selectedMap]->getMapWidth() * scaleX + sobra/2)
			*x = mapVect[selectedMap]->getMapWidth() - 1;

		else
			*x = ((*x - sobra/2) * mapVect[selectedMap]->getMapWidth()) / (double)(tamTexW - sobra);
	}

	return true;
}

bool MapFrame::handleInternalSdlEvent(SDL_Event &e)
{
	bool updateMapTexture = false;

	switch(e.type)
	{
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
				case SDLK_UP:
					// up always shows sea (without increasing sealvl on first press)
					if(mapTexture->getSeaRenderMode() != WITH_SEA)
					{
						mapTexture->setSeaRenderMode(WITH_SEA);
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
					if(e.key.keysym.mod & KMOD_SHIFT && mapTexture->getSeaRenderMode() != NO_SEA)
					{
						mapTexture->setSeaRenderMode(NO_SEA);
						mapTexture->setLandRenderMode(FIXED);
						updateMapTexture = true;
					}

					// down = -1 sealvl when with_sea
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapVect[selectedMap]->getSeaLvl() - 1 > 0 && mapTexture->getSeaRenderMode() == WITH_SEA)
					{
						mapVect[selectedMap]->decreaseSeaLvl();
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();

						updateMapTexture = true;
					}
				break;

				case SDLK_z:
					if(mapTexture->getSeaRenderMode() != NO_SEA)
					{
						mapTexture->setLandRenderMode(VARYING_HIGHEST);
						updateMapTexture = true;
					}
				break;

				case SDLK_x:
					if(mapTexture->getSeaRenderMode() != NO_SEA)
					{
						mapTexture->setLandRenderMode(VARYING_MAX);
						updateMapTexture = true;
					}
				break;

				case SDLK_c:
					if(mapTexture->getSeaRenderMode() != NO_SEA)
					{
						mapTexture->setLandRenderMode(FIXED);
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
						mapTexture->setMap(mapVect[selectedMap]);
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
						mapTexture->setMap(mapVect[selectedMap]);
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
						mapTexture->setMap(mapVect[selectedMap]);
						cout << endl << endl << "Map 3" << endl;
						cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();
						updateMapTexture = true;
					}
				break;

				default:
					// se evento foi keyboard mas não era nenhum dos tratados, retorna evento pro pai
					return false;
				break;
			}
		break;

		case SDL_MOUSEBUTTONUP:
			if(e.button.button == SDL_BUTTON_RIGHT)
			{
				int x, y;
				
				if(mapPosFromMouse(&x, &y))
				{
//walkWindow.setMap(mapVect[selectedMap]);
//walkWindow.show(); // must happen before setPos
//walkWindow.setPos(x, y);
				}
			}
		break;

		case SDL_MOUSEMOTION:
			updateMouseText();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		default:
			// se era outro tipo de evento, não retorna pro pai
			return true;
		break;
	}

	if(updateMapTexture)
	{
		mapTexture->update();
		publishUiEvent(UIEVT_CONTENTSCHANGED);
	}

	return true;
}

bool MapFrame::handleUiEvent(int evtId)
{
	switch(evtId)
	{
		case UIEVT_BTCLICKEDNOISE0:
			selectedNoise = 0;
		break;

		case UIEVT_BTCLICKEDNOISE1:
			selectedNoise = 1;
		break;

		case UIEVT_BTCLICKEDNOISE2:
			selectedNoise = 2;
		break;

		case UIEVT_BTCLICKEDMAP0:
			selectedMap = 0;
			mapTexture->setMap(mapVect[selectedMap]);
			cout << endl << endl << "Map " << selectedMap+1 << endl;
			cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();

			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		case UIEVT_BTCLICKEDMAP1:
			selectedMap = 1;
			mapTexture->setMap(mapVect[selectedMap]);
			cout << endl << endl << "Map " << selectedMap+1 << endl;
			cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();

			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		case UIEVT_BTCLICKEDMAP2:
			selectedMap = 2;
			mapTexture->setMap(mapVect[selectedMap]);
			cout << endl << endl << "Map " << selectedMap+1 << endl;
			cout << "Sea Level : " << setw(3) << setfill('0') << mapVect[selectedMap]->getSeaLvl();

			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		return true;
	}

	return false;
}
