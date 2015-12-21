#include "CustomSdlWindows/NoiseWindow.h"

#include "Map/Map.h"
#include "Map/Noises/DiamSqNoise.h"
#include "Map/Noises/OpenSimplexNoise.h"

#include "Ui/UiObject.h"
#include "CustomUiObjects/MapFrame.h"
#include "DefaultUiObjects/UiPanel.h"
#include "DefaultUiObjects/UiButton.h"
#include "DefaultUiObjects/UiLabel.h"

#include "Ui/EvtAggr.h"
#include "Ui/UiEvents/UiCode.h"
#include "Ui/UiEvents/NoiseInfoRequest.h"

#include "MyUtils.h"

#include <sstream>
#include <string>


#include "CustomSdlTextures/MapTexture.h" // TEMPORARY: FOR THE DEFINES


NoiseWindow::NoiseWindow() : 
	SdlWindow(	"WorldGen", 20, 40, 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window size 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window resolution
				SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE) // superclass window constructor
{
	mapVector = {std::make_shared<Map>(1025, 1025), 
				std::make_shared<Map>(1025, 1025), 
				std::make_shared<Map>(1025, 1025)};
	selectedMap = mapVector.at(0);

	noiseVector.push_back(std::make_unique<OpenSimplexNoise>(selectedMap, octaves, freq, persistence, freqDiv));
	noiseVector.push_back(std::make_unique<DiamSqNoise>(selectedMap));
	selectedNoiseIdx = 1; 

	normalizedLevel = 20;

	subscribeTkUiCode = EvtAggr::subscribe<UiCode>( [&](UiCode &c){ handleUiCode(c); } );

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	createGui();

	setWindowSdlEvtHandler([&](SDL_Event &e){return customSdlEvtHandler(e);});
}

NoiseWindow::~NoiseWindow()
{
	EvtAggr::unsubscribe<UiCode>(subscribeTkUiCode);
}

bool NoiseWindow::customSdlEvtHandler(SDL_Event &e)
{
	switch(e.type)
	{
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
				case SDLK_UP:
					if(!(e.key.keysym.mod & KMOD_SHIFT) && selectedMap->getSeaLevel() + 1 <= selectedMap->getHighestH())
					{
						selectedMap->increaseSeaLevel();
						updateMap();
					}
				break;

				case SDLK_DOWN:
					if(!(e.key.keysym.mod & KMOD_SHIFT) && selectedMap->getSeaLevel() - 1 >= 0)
					{
						selectedMap->decreaseSeaLevel();
						updateMap();
					}
				break;

				case SDLK_1:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(0);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectMap(0);
				break;

				case SDLK_2:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(1);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectMap(1);
				break;

				case SDLK_3:
					if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectMap(2);
				break;

				case SDLK_n:
					normalizeMap(normalizedLevel);
				break;

				case SDLK_r:
					runNoise();
				break;

				// if event was from keyboard but wasn't handled, returns to parent
				default:
					return false;
				break;
			}
		break;
	}
	
	return true;
}

void NoiseWindow::handleUiCode(UiCode &c)
{
	if(c.code == UIEVT_NOISEINFOUPDATED && noiseInfoText != NULL)
	{
		NoiseInfoRequest req;
		EvtAggr::publish<NoiseInfoRequest*>(&req);

		std::stringstream ss;
		ss << "";
		
		ss << "\n\nOct: " << req.oct
		<< "\nFrequency: " << req.freq 
		<< "\nPersistency: " << req.pers
		<< "\nfDiv: " << req.fDiv;
		
		noiseInfoText->setText(ss.str());
	}
}

void NoiseWindow::updateMap()
{
	mapFrame->updateTexture();
	updateMapInfo();
}

void NoiseWindow::updateMapInfo()
{
	if(mapInfoText != NULL)
	{
		std::stringstream ss;
		
		ss << "Noise: \n" 
		<< noiseVector[selectedNoiseIdx]->name
		<< "\n\nMap " << "NUM";
		if(noiseVector[selectedNoiseIdx]->getPercentComplete() != 0 && noiseVector[selectedNoiseIdx]->getPercentComplete() != 100)
			ss << " - " << noiseVector[selectedNoiseIdx]->getPercentComplete() << "%";
		ss << "\nSea Level: " << selectedMap->getSeaLevel()
		<< "\n\nHighest: " << selectedMap->getHighestH()
		<< "\nLowest: " << selectedMap->getLowestH();
		
		mapInfoText->setText(ss.str());
	}
}

void NoiseWindow::selectMap(int index)
{
	selectedMap = mapVector.at(index);
	updateMap();
	mapFrame->resetZoom();
}

void NoiseWindow::selectNoise(int index)
{
	selectedNoiseIdx = index;
	updateMapInfo();
}

void NoiseWindow::runNoise()
{
	bool update = false;
	int updateAt = 0;

	noiseVector[selectedNoiseIdx]->setMap(selectedMap); // sets target map on noise and resets

	mapFrame->setSeaRenderMode(NO_SEA);  // no sea while not done
	mapFrame->setLandRenderMode(FIXED);

	while(noiseVector[selectedNoiseIdx]->getPercentComplete() < 100) // noise iterations
	{
		noiseVector[selectedNoiseIdx]->runOnce();

		int percentComplete = noiseVector[selectedNoiseIdx]->getPercentComplete();

		if(percentComplete >= updateAt || percentComplete >= 100)
		{
			update = true;
			updateAt += UPDATE_AT_PERCENT;

			if(percentComplete >= 100)
			{
				mapFrame->setSeaRenderMode(WITH_SEA);
				mapFrame->setLandRenderMode(VARYING_HIGHEST);

				selectedMap->setSeaLevel(SEA_LEVEL);
			}
		}

		if(update)
		{
			updateMap();
			doRefresh();
			update = false;
		}

		SDL_PumpEvents();
	}
}

void NoiseWindow::normalizeMap(int n)
{
	selectedMap->normalize(n);
	
	// reset sea level after normalization
	selectedMap->setSeaLevel((selectedMap->getHighestH() / 2 ) - 1);
	
	updateMap();
}

// UI
void NoiseWindow::createGui()
{
	std::shared_ptr<SdlTexture> standardBtTexture = MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png");
	std::shared_ptr<SdlTexture> sidebarBg = createDrawnTexture(SIDEBAR_WIDTH, windowUiManager->getHeight(), 0, 126, 126, 255);

	mapFrame = new MapFrame(0, 0, windowUiManager->getWidth() - SIDEBAR_WIDTH, windowUiManager->getHeight(), selectedMap);

	UiPanel *sidebar = new UiPanel(windowUiManager->getWidth() - SIDEBAR_WIDTH, 0, sidebarBg);

	// Text
	mapInfoText = new UiLabel(30, 150, "", 18, 175, 0, 0);
	sidebar->addChild(mapInfoText);
	
	noiseInfoText = new UiLabel(30, windowUiManager->getHeight() - 310, "", 18, 175, 0, 0);
	sidebar->addChild(noiseInfoText);

	// Map selectors
	sidebar->addChild(new UiButton(	30, 30, 
									30, 30,
									new UiLabel(0, 0, "1", 16, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectMap(0); } ));

	sidebar->addChild(new UiButton(	90, 30, 
									30, 30, 
									new UiLabel(0, 0, "2", 16, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectMap(1); } ));

	sidebar->addChild(new UiButton(	150, 30, 
									30, 30, 
									new UiLabel(0, 0, "3", 16, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectMap(2); } ));

	// Noise selectors
	sidebar->addChild(new UiButton(	30, 80, 
									70, 30, 
									new UiLabel(0, 0, "OpSim", 14, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectNoise(0); } ));

	sidebar->addChild(new UiButton(	110, 80, 
									70, 30, 
									new UiLabel(0, 0, "DiamSq", 14, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectNoise(1); } ));

	// Run button
	sidebar->addChild(new UiButton(	30, windowUiManager->getHeight() - 60, 
									150, 30, 
									new UiLabel(0, 0, "RUN", 22, 235, 235, 235), 
									standardBtTexture, 
									[&](){ runNoise(); } ));

	// Normalization buttons
	static UiLabel *nLevel = new UiLabel(48, windowUiManager->getHeight() - 100, ALIGN_BOTTOM_CENTER, "20", 18, 235, 235, 235);
	sidebar->addChild(nLevel);

	sidebar->addChild(new UiButton(	50, windowUiManager->getHeight() - 95, 
									15, 15, 
									new UiLabel(0, 0, "+", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	normalizedLevel++;
											std::stringstream ss;
											ss << normalizedLevel;
											nLevel->setText(ss.str()); } ));

	sidebar->addChild(new UiButton(	30, windowUiManager->getHeight() - 95, 
									15, 15, 
									new UiLabel(0, 0, "-", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){ normalizedLevel--;
											std::stringstream ss;
											ss << normalizedLevel;
											nLevel->setText(ss.str()); } ));

	sidebar->addChild(new UiButton(	80, windowUiManager->getHeight() - 115, 
									100, 30, 
									new UiLabel(0, 0, "Normalize", 14, 235, 235, 235), 
									standardBtTexture, 
									[&](){ normalizeMap(normalizedLevel); } ));


	// >>>>>>> TO DO: FIX NOISE SIDE SETTINGS CHANGES THAT GET LOST WHEN NOISES ARE RECONSTRUCTED <<<<<<<
	// Noise adjust buttons
	sidebar->addChild(new UiButton(	30, windowUiManager->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "f", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FREQDECREASE)); } ));

	sidebar->addChild(new UiButton(	50, windowUiManager->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "F", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FREQINCREASE)); } ));

	sidebar->addChild(new UiButton(	75, windowUiManager->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "p", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_PERSDECREASE)); } ));

	sidebar->addChild(new UiButton(	95, windowUiManager->getHeight() - 145, 
									15, 15,  
									new UiLabel(0, 0, "P", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_PERSINCREASE)); } ));

	sidebar->addChild(new UiButton(	120, windowUiManager->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "d", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FDIVDECREASE)); } ));

	sidebar->addChild(new UiButton(	140, windowUiManager->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "D", 12, 235, 235, 235), 
									standardBtTexture, 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FDIVINCREASE)); } ));

	mapFrame->setUiObjectLogicalSize(windowUiManager->getWidth(), windowUiManager->getHeight());
	mapFrame->addChild(sidebar);
	
	windowUiManager->addChild(mapFrame);

	// must happen after the MapFrame has been added to UiManager (so it has a parentUiManager)
	mapFrame->init();
	updateMapInfo();
}

std::shared_ptr<SdlTexture> NoiseWindow::createDrawnTexture(int width, int height, int r, int g, int b, int a)
{
	std::shared_ptr<SdlTexture> tex = std::make_shared<SdlTexture>(SDL_CreateTexture(	getRenderer(), 
																						SDL_PIXELFORMAT_RGBA8888, 
																						SDL_TEXTUREACCESS_TARGET, 
																						width, height));

	tex->setAsRenderTarget(getRenderer());
	SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
	SDL_RenderClear(getRenderer());
	tex->releaseRenderTarget(getRenderer());

	return tex;
}
