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

MapFrame::MapFrame(SDL_Renderer *r, int x, int y, int w, int h, Map* mapArr[], int num) : 
	UiObject(r, x, y, w, h, NULL,
			[&](SDL_Event &e){return handleInternalSdlEvent(e);})
{
	// make mapFrame observe events, so it may respond to other UiObjects
	addUiEventObserver(this);

	mapArray = mapArr;
	numMaps = num;
	selectedMap = 0;

	noiseArray[0] = new OpenSimplexNoise(mapArray[0], octaves, freq, persistence, freqDiv);
	noiseArray[1] = new DiamSqNoise(mapArray[0]);
	noiseArray[2] = new MyNoise(mapArray[0]);
	selectedNoise = 0;

	mapTexture = new MapTexture(getRenderer(), mapArray[selectedMap]);
	mapTexture->update();
	setUiObjectTexture(mapTexture);

	mouseText = new MouseHeightText(getRenderer());
	addChild(mouseText);
	
	publishUiEvent(UIEVT_CONTENTSCHANGED);
}

MapFrame::~MapFrame()
{
	for(int i = 0; i < 3; i++)
		delete noiseArray[i];

	removeUiEventObserver(this);
}

void MapFrame::runNoise()
{
	bool updateMapTexture = false;
	int shownPercent = -1;

	noiseArray[selectedNoise]->setMap(mapArray[selectedMap]);

	cout << endl << endl;

	while(noiseArray[selectedNoise]->getPercentComplete() < 100) // noise iterations
	{
		noiseArray[selectedNoise]->runOnce();

		// update only once per percent
		if(noiseArray[selectedNoise]->getPercentComplete() != shownPercent)
		{
			shownPercent = noiseArray[selectedNoise]->getPercentComplete();

			cout << "\b\b\b\b" << shownPercent << "%";

			if(shownPercent == 100) // show highest at each phase (if appliable)
				cout << endl 
				<< endl << "Highest point: " << mapArray[selectedMap]->getHighestH()
				<< endl << "Lowest point: " << mapArray[selectedMap]->getLowestH() << endl << endl;
			
			if(shownPercent % UPDATE_AT_PERCENT == 0)
			{
				if(noiseArray[selectedNoise]->getPercentComplete() < 100) // no sea while not done
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
					cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();
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

	mapArray[selectedMap]->setSeaLvl(SEA_LEVEL);
}

void MapFrame::resetNoise()
{
	noiseArray[selectedNoise]->reset();
}

void MapFrame::updateMouseText()
{
	int mapX, mapY;
	
	if(mapPosFromMouse(&mapX, &mapY))
	{
		int h = mapArray[selectedMap]->Tile(mapX, mapY).getH();
		string text;

		if(mapTexture->getSeaRenderMode() == WITH_SEA && h <= mapArray[selectedMap]->getSeaLvl())
			text = "Sea";

		else
		{
			stringstream ss;
			ss << h;
			text = ss.str();
		}

		mouseText->update(text);

		int x, y;
		UiObject::getRelativeMousePos(this, &x, &y);
		mouseText->setUiObjectOffset(x, y - 30);
	}
}

bool MapFrame::mapPosFromMouse(int *x, int *y)
{
	if(!UiObject::getRelativeMousePos(this, x, y))
		return false;

	int mapFrameWidth = getWidth();
	int mapFrameHeight = getHeight();

	// mousePos / frameSize = mapPos / mapSize --> 
	// mapPos = (mousePos / frameSize) * mapSize
	*x = (*x / (double)mapFrameWidth) * mapArray[selectedMap]->getMapWidth();
	*y = (*y / (double)mapFrameHeight) * mapArray[selectedMap]->getMapHeight();

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
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapArray[selectedMap]->getSeaLvl() + 1 < mapArray[selectedMap]->getHighestH())
					{
						mapArray[selectedMap]->increaseSeaLvl();
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();
						
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
					else if(!(e.key.keysym.mod & KMOD_SHIFT) && mapArray[selectedMap]->getSeaLvl() - 1 > 0 && mapTexture->getSeaRenderMode() == WITH_SEA)
					{
						mapArray[selectedMap]->decreaseSeaLvl();
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();

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
					mapArray[selectedMap]->normalize(n);
					mapArray[selectedMap]->setSeaLvl((mapArray[selectedMap]->getHighestH() / 2 ) - 1);
					cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();
					updateMapTexture = true;
				break;

				case SDLK_1:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectedNoise = 0;

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
					{
						selectedMap = 0;
						mapTexture->setMap(mapArray[selectedMap]);
						cout << endl << endl << "Map 1" << endl;
						cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();
						updateMapTexture = true;
					}
				break;

				case SDLK_2:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectedNoise = 1;

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
					{
						selectedMap = 1;
						mapTexture->setMap(mapArray[selectedMap]);
						cout << endl << endl << "Map 2" << endl;
						cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();
						updateMapTexture = true;
					}
				break;

				case SDLK_3:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectedNoise = 2;

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
					{
						selectedMap = 2;
						mapTexture->setMap(mapArray[selectedMap]);
						cout << endl << endl << "Map 3" << endl;
						cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();
						updateMapTexture = true;
					}
				break;

				default:
					// if event was from keyboard but wasn't handled, returns to parent
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
//walkWindow.setMap(mapArray[selectedMap]);
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
			// if event wasn't from keyboard, doesn't return to parent
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
			mapTexture->setMap(mapArray[selectedMap]);
			cout << endl << endl << "Map " << selectedMap+1 << endl;
			cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();

			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		case UIEVT_BTCLICKEDMAP1:
			selectedMap = 1;
			mapTexture->setMap(mapArray[selectedMap]);
			cout << endl << endl << "Map " << selectedMap+1 << endl;
			cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();

			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		case UIEVT_BTCLICKEDMAP2:
			selectedMap = 2;
			mapTexture->setMap(mapArray[selectedMap]);
			cout << endl << endl << "Map " << selectedMap+1 << endl;
			cout << "Sea Level : " << setw(3) << setfill('0') << mapArray[selectedMap]->getSeaLvl();

			mapTexture->update();
			publishUiEvent(UIEVT_CONTENTSCHANGED);
		break;

		return true;
	}

	return false;
}
