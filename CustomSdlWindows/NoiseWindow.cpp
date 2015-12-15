#include "CustomSdlWindows/NoiseWindow.h"

#include "Map/Map.h"

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

NoiseWindow::NoiseWindow(Map* mapArr[], int num) : 
	SdlWindow(	"WorldGen", 20, 40, 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window size 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window resolution
				SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE) // superclass window constructor
{
	EvtAggr::subscribe<UiCode>( [&](UiCode &c){ customUiEventHandler(c); } );

	EvtAggr::subscribe<MapInfoUpdate>( [&](MapInfoUpdate &i){ updateMapInfo(i); } );

	mapArray = mapArr;
	numMaps = num;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	createGui();
}

NoiseWindow::~NoiseWindow()
{
	EvtAggr::unsubscribe<UiCode>( [&](UiCode &c){ customUiEventHandler(c); } );

	EvtAggr::unsubscribe<MapInfoUpdate>( [&](MapInfoUpdate &i){ updateMapInfo(i); } );
}

void NoiseWindow::customUiEventHandler(UiCode &c)
{
	if(c.code == UIEVT_RUNNOISEUPDATE)
	{
		doRefresh();
	}

	if(c.code == UIEVT_NOISEINFOUPDATED)
	{
		NoiseInfoRequest req;
		EvtAggr::publish<NoiseInfoRequest*>(&req);

		std::stringstream ss;
		
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
		
		if(info.percentComplete != 0 && info.percentComplete != 100)
			ss << "Noise: \n" 
			<< info.noiseName
			<< "\n\nMap " << info.mapNum << " - " << info.percentComplete 
			<< "%\nSea Level: " << info.seaLevel 
			<< "\n\nHighest: " << info.highestH 
			<< "\nLowest: " << info.lowestH;
		else
			ss << "Noise: \n" 
			<< info.noiseName
			<< "\n\nMap " << info.mapNum
			<< "\nSea Level: " << info.seaLevel 
			<< "\n\nHighest: " << info.highestH 
			<< "\nLowest: " << info.lowestH;
		
		mapInfoText->setText(ss.str());
	}
}

// UI

MapFrame *mapFrame = nullptr;

void NoiseWindow::createGui()
{
	// before mapFrame so it can receive the first map info event with updateMapInfo() above
	mapInfoText = new UiLabel(30, 150, "", 18, 175, 0, 0);
	noiseInfoText = new UiLabel(30, windowUi->getHeight() - 310, "", 18, 175, 0, 0);
	
	mapFrame = new MapFrame(windowUi, 0, 0, windowUi->getWidth() - SIDEBAR_WIDTH, windowUi->getHeight(), mapArray, numMaps);
	mapFrame->setUiObjectLogicalSize(windowUi->getWidth(), windowUi->getHeight());

	UiPanel *sidebar = new UiPanel(windowUi->getWidth() - SIDEBAR_WIDTH, 0, 
									createDrawnTexture(SIDEBAR_WIDTH, windowUi->getHeight(), 0, 126, 126, 255));

	sidebar->addChild(mapInfoText);
	sidebar->addChild(noiseInfoText);

	sidebar->addChild(new UiButton(	30, 30, 
									30, 30,
									new UiLabel(0, 0, "1", 16, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->selectMap(0); } ));

	sidebar->addChild(new UiButton(	90, 30, 
									30, 30, 
									new UiLabel(0, 0, "2", 16, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->selectMap(1); } ));

	sidebar->addChild(new UiButton(	150, 30, 
									30, 30, 
									new UiLabel(0, 0, "3", 16, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->selectMap(2); } ));

	sidebar->addChild(new UiButton(	30, 80, 
									70, 30, 
									new UiLabel(0, 0, "OpSim", 14, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->selectNoise(0); } ));

	sidebar->addChild(new UiButton(	110, 80, 
									70, 30, 
									new UiLabel(0, 0, "DiamSq", 14, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->selectNoise(1); } ));

	sidebar->addChild(new UiButton(	30, windowUi->getHeight() - 60, 
									150, 30, 
									new UiLabel(0, 0, "RUN", 22, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->runNoise(); } ));

	static int normalizedLevel = 20;
	static UiLabel *nLevel = new UiLabel(48, windowUi->getHeight() - 100, ALIGN_BOTTOM_CENTER, "20", 18, 235, 235, 235);

	sidebar->addChild(nLevel);

	sidebar->addChild(new UiButton(	50, windowUi->getHeight() - 95, 
									15, 15, 
									new UiLabel(0, 0, "+", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	normalizedLevel++;
											std::stringstream ss;
											ss << normalizedLevel;
											nLevel->setText(ss.str()); } ));

	sidebar->addChild(new UiButton(	30, windowUi->getHeight() - 95, 
									15, 15, 
									new UiLabel(0, 0, "-", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ normalizedLevel--;
											std::stringstream ss;
											ss << normalizedLevel;
											nLevel->setText(ss.str()); } ));

	sidebar->addChild(new UiButton(	80, windowUi->getHeight() - 115, 
									100, 30, 
									new UiLabel(0, 0, "Normalize", 14, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){ mapFrame->normalizeMap(normalizedLevel); } ));


	//noise adjust buttons
	sidebar->addChild(new UiButton(	30, windowUi->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "f", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FREQDECREASE)); } ));

	sidebar->addChild(new UiButton(	50, windowUi->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "F", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FREQINCREASE)); } ));

	sidebar->addChild(new UiButton(	75, windowUi->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "p", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_PERSDECREASE)); } ));

	sidebar->addChild(new UiButton(	95, windowUi->getHeight() - 145, 
									15, 15,  
									new UiLabel(0, 0, "P", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_PERSINCREASE)); } ));

	sidebar->addChild(new UiButton(	120, windowUi->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "d", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FDIVDECREASE)); } ));

	sidebar->addChild(new UiButton(	140, windowUi->getHeight() - 145, 
									15, 15, 
									new UiLabel(0, 0, "D", 12, 235, 235, 235), 
									std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btSprite.png")), 
									[&](){	EvtAggr::publish<UiCode>(UiCode(UIEVT_FDIVINCREASE)); } ));

	mapFrame->addChild(sidebar);
	windowUi->addChild(mapFrame);
}

std::shared_ptr<SdlTexture> NoiseWindow::createDrawnTexture(int width, int height, int r, int g, int b, int a)
{
	std::shared_ptr<SdlTexture> tex = std::make_shared<SdlTexture>(SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height));
	tex->setAsRenderTarget(getRenderer());
	SDL_SetRenderDrawColor(getRenderer(), r, g, b, a);
	SDL_RenderClear(getRenderer());
	tex->releaseRenderTarget(getRenderer());

	return tex;
}
