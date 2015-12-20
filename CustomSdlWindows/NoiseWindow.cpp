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
#include "Ui/UiEvents/MapInfoUpdate.h"
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
	mapVector = {Map(1025, 1025), 
				Map(1025, 1025), 
				Map(1025, 1025)};

	selectedMap = &mapVector.at(0);

	normalizedLevel = 20;

	selectedNoiseId = 1; 
	selectedNoise = std::make_unique<DiamSqNoise>(*selectedMap);


	subscribeTkUiCode = EvtAggr::subscribe<UiCode>( [&](UiCode &c){ customUiEventHandler(c); } );
	
	subscribeTkMapInfoUpdate = EvtAggr::subscribe<MapInfoUpdate>( [&](MapInfoUpdate &i){ updateMapInfo(i); } );

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	createGui();

	setWindowSdlEvtHandler([&](SDL_Event &e){return customSdlEvtHandler(e);});
}

NoiseWindow::~NoiseWindow()
{
	EvtAggr::unsubscribe<UiCode>(subscribeTkUiCode);

	EvtAggr::unsubscribe<MapInfoUpdate>(subscribeTkMapInfoUpdate);
}

bool NoiseWindow::customSdlEvtHandler(SDL_Event &e)
{
	switch(e.type)
	{
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
				case SDLK_1:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(0);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectedMap = &mapVector.at(0); mapFrame->reset();
				break;

				case SDLK_2:
					if(e.key.keysym.mod & KMOD_SHIFT)
						selectNoise(1);

					else if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectedMap = &mapVector.at(1); mapFrame->reset();
				break;

				case SDLK_3:
					if(!(e.key.keysym.mod & KMOD_SHIFT))
						selectedMap = &mapVector.at(2); mapFrame->reset();
				break;

				case SDLK_n:
					mapFrame->normalizeMap(normalizedLevel);
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

void NoiseWindow::customUiEventHandler(UiCode &c)
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

void NoiseWindow::updateMapInfo(MapInfoUpdate &info)
{
	if(mapInfoText != NULL)
	{
		std::stringstream ss;
		
		if(selectedNoise->getPercentComplete() != 0 && selectedNoise->getPercentComplete() != 100)
			ss << "Noise: \n" 
			<< selectedNoise->name
			<< "\n\nMap " << "NUM" << " - " << selectedNoise->getPercentComplete()
			<< "%\nSea Level: " << info.seaLevel 
			<< "\n\nHighest: " << info.highestH 
			<< "\nLowest: " << info.lowestH;
		else
			ss << "Noise: \n" 
			<< selectedNoise->name
			<< "\n\nMap " << "NUM"
			<< "\nSea Level: " << info.seaLevel 
			<< "\n\nHighest: " << info.highestH 
			<< "\nLowest: " << info.lowestH;
		
		mapInfoText->setText(ss.str());
	}
}

void NoiseWindow::selectNoise(int id)
{
	selectedNoiseId = id;

	if(selectedNoiseId == 0)
		selectedNoise = std::make_unique<OpenSimplexNoise>(*selectedMap, octaves, freq, persistence, freqDiv);

	else if(selectedNoiseId == 1)
		selectedNoise = std::make_unique<DiamSqNoise>(*selectedMap);

	// >>>>>>> TO DO: GET RID OF PUBLISHMAPINFO() <<<<<<<
	mapFrame->publishMapInfo();
}

void NoiseWindow::runNoise()
{
	if(selectedNoiseId == 0)
		selectedNoise = std::make_unique<OpenSimplexNoise>(*selectedMap, octaves, freq, persistence, freqDiv);

	else if(selectedNoiseId == 1)
		selectedNoise = std::make_unique<DiamSqNoise>(*selectedMap);

	else
	{
		std::cout << "MapFrame::runNoise() called with an invalid selectedNoiseId." << std::endl;
		return;
	}

	bool updateMapTexture = false;
	int shownPercent = 0, updateAt = 0;

	mapFrame->setSeaRenderMode(NO_SEA);  // no sea while not done
	mapFrame->setLandRenderMode(FIXED);

	while(selectedNoise->getPercentComplete() < 100) // noise iterations
	{
		selectedNoise->runOnce();

		// update only once per percent
		if(selectedNoise->getPercentComplete() != shownPercent)
		{
			shownPercent = selectedNoise->getPercentComplete();

			if(shownPercent >= updateAt || shownPercent >= 100)
			{
				updateAt += UPDATE_AT_PERCENT;
				updateMapTexture = true;

				if(shownPercent >= 100)
				{
					mapFrame->setSeaRenderMode(WITH_SEA);
					mapFrame->setLandRenderMode(VARYING_HIGHEST);

					selectedMap->setSeaLevel(SEA_LEVEL);
				}
			}
		}

		if(updateMapTexture)
		{
			mapFrame->reset();
			doRefresh();
			updateMapTexture = false;
		}

		SDL_PumpEvents();
	}

	mapFrame->publishMapInfo();
}

// UI

void NoiseWindow::createGui()
{
	std::shared_ptr<SdlTexture> standardBtTexture = MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png");
	std::shared_ptr<SdlTexture> sidebarBg = createDrawnTexture(SIDEBAR_WIDTH, windowUiManager->getHeight(), 0, 126, 126, 255);

	mapFrame = new MapFrame(0, 0, windowUiManager->getWidth() - SIDEBAR_WIDTH, windowUiManager->getHeight(), &selectedMap);

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
									[&](){ selectedMap = &mapVector.at(0); mapFrame->reset(); } ));

	sidebar->addChild(new UiButton(	90, 30, 
									30, 30, 
									new UiLabel(0, 0, "2", 16, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectedMap = &mapVector.at(1); mapFrame->reset(); } ));

	sidebar->addChild(new UiButton(	150, 30, 
									30, 30, 
									new UiLabel(0, 0, "3", 16, 235, 235, 235), 
									standardBtTexture, 
									[&](){ selectedMap = &mapVector.at(2); mapFrame->reset(); } ));

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
									[&](){ mapFrame->normalizeMap(normalizedLevel); } ));


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
