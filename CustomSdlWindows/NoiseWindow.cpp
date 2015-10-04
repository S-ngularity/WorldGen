#include "CustomSdlWindows/NoiseWindow.h"

#include "Map/Map.h"

#include "Ui/UiObject.h"
#include "CustomUiObjects/MapFrame.h"
#include "DefaultUiObjects/UiPanel.h"
#include "DefaultUiObjects/UiButton.h"
#include "DefaultUiObjects/UiLabel.h"

#include "Ui/EventAggregator.h"
#include "Ui/UiEvents/UiEventCode.h"
#include "Ui/UiEvents/MapInfoUpdate.h"

#include "MyUtils.h"

#include <sstream>
#include <string>

NoiseWindow::NoiseWindow(Map* mapArr[], int num) : 
	SdlWindow(	"WorldGen", 20, 40, 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window size 
				SCREEN_WIDTH, SCREEN_HEIGHT, // window resolution
				SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE) // superclass window constructor
{
	EventAggregator::Instance().getEvent<UiEventCode>().subscribe(
															[&](UiEventCode &c){ customUiEventHandler(c); });

	EventAggregator::Instance().getEvent<MapInfoUpdate>().subscribe(
															[&](MapInfoUpdate &i){ updateMapInfo(i); });

	mapArray = mapArr;
	numMaps = num;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	createGui();
}

NoiseWindow::~NoiseWindow()
{
	EventAggregator::Instance().getEvent<UiEventCode>().unsubscribe(
															[&](UiEventCode &c){ customUiEventHandler(c); });

	EventAggregator::Instance().getEvent<MapInfoUpdate>().unsubscribe(
															[&](MapInfoUpdate &i){ updateMapInfo(i); });
}

void NoiseWindow::customUiEventHandler(UiEventCode &c)
{
	if(c.code == UIEVT_RUNNOISEUPDATE)
	{
		doRefresh();
	}
}

void NoiseWindow::updateMapInfo(MapInfoUpdate &info)
{
	if(mapInfoText != NULL)
	{
		std::stringstream ss;
		
		if(info.percentComplete != 100)
			ss << "Map " << info.mapNum << " - " << info.percentComplete << "%\nSea Level: " << info.seaLevel << "\n\nHighest: " << info.highestH << "\nLowest: " << info.lowestH;
		else
			ss << "Map " << info.mapNum << "\nSea Level: " << info.seaLevel << "\n\nHighest: " << info.highestH << "\nLowest: " << info.lowestH;
		
		mapInfoText->setText(ss.str());
	}
}

// UI

MapFrame *mapFrame = nullptr;

void NoiseWindow::createGui()
{
	// before mapFrame so it can receive the first map info event with updateMapInfo() above
	mapInfoText = new UiLabel(30, 150, "", 18, 175, 0, 0);
	
	mapFrame = new MapFrame(windowUi, 0, 0, windowUi->getWidth() - SIDEBAR_WIDTH, windowUi->getHeight(), mapArray, numMaps);
	mapFrame->setUiObjectLogicalSize(windowUi->getWidth(), windowUi->getHeight());

	UiPanel *sidebar = new UiPanel(windowUi->getWidth() - SIDEBAR_WIDTH, 0, 
									createDrawnTexture(SIDEBAR_WIDTH, windowUi->getHeight(), 0, 126, 126, 255));

	sidebar->addChild(mapInfoText);

	sidebar->addChild(new UiButton(30, 30, 
									NULL, 
									createDrawnTexture(30, 30, 0, 255, 0, 255), 
									createDrawnTexture(30, 30, 115, 255, 115, 255), 
									createDrawnTexture(30, 30, 0, 205, 0, 255), 
									[&](){ mapFrame->selectMap(0); } ));

	sidebar->addChild(new UiButton(80, 30, 
									NULL, 
									createDrawnTexture(30, 30, 0, 255, 0, 255), 
									createDrawnTexture(30, 30, 115, 255, 115, 255), 
									createDrawnTexture(30, 30, 0, 205, 0, 255), 
									[&](){ mapFrame->selectMap(1); } ));

	sidebar->addChild(new UiButton(130, 30, 
									NULL, 
									createDrawnTexture(30, 30, 0, 255, 0, 255), 
									createDrawnTexture(30, 30, 115, 255, 115, 255), 
									createDrawnTexture(30, 30, 0, 205, 0, 255), 
									[&](){ mapFrame->selectMap(2); } ));

	sidebar->addChild(new UiButton(55, 80, 
									NULL, 
									createDrawnTexture(30, 30, 0, 255, 0, 255), 
									createDrawnTexture(30, 30, 115, 255, 115, 255), 
									createDrawnTexture(30, 30, 0, 205, 0, 255), 
									[&](){ mapFrame->selectNoise(0); } ));

	sidebar->addChild(new UiButton(105, 80, 
									NULL, 
									createDrawnTexture(30, 30, 0, 255, 0, 255), 
									createDrawnTexture(30, 30, 115, 255, 115, 255), 
									createDrawnTexture(30, 30, 0, 205, 0, 255), 
									[&](){ mapFrame->selectNoise(1); } ));

	sidebar->addChild(new UiButton(30, windowUi->getHeight() - 60, 
									new UiLabel(0, 0, "RUN", 22, 255, 0, 0), 
									createDrawnTexture(130, 30, 0, 255, 0, 255), 
									createDrawnTexture(130, 30, 115, 255, 115, 255), 
									createDrawnTexture(130, 30, 0, 205, 0, 255), 
									//std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btNormal.png"), 130, 30), 
									//std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btHover.png"), 130, 30), 
									//std::make_shared<SdlTexture>(MyUtils::loadTexture(getRenderer(), "Resources\\btPressed.png"), 130, 30), 
									[&](){ mapFrame->runNoise(); } ));

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
