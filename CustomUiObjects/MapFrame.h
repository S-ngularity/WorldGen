#ifndef MAPFRAME
#define MAPFRAME

#include "Ui/UiObject.h"

#include <memory>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class MapTexture;
class SdlTexture;
class UiManager;
class UiLabel;

class MapFrame : public UiObject
{
	private:
		std::unique_ptr<MapTexture> mapTexture;
		
		std::shared_ptr<SdlTexture> frameTexture;

		Map **ptrToMap;

		UiLabel *mouseTooltip;

		bool clickHappenedHere;
		int mouseLastX, mouseLastY;
		double mouseXDragOffset, mouseYDragOffset;
		int mapOffset;

		int zoomX, zoomY, zoomW, zoomH;

		void updateMouseText();
		
		bool mapPosFromMouse(int *x, int *y);

		bool customSdlEvtHandler(SDL_Event &e);


		void preRenderProcedure();

	public:
		MapFrame(int x, int y, int w, int h, Map **mapPtr);

		// Initializes the noises/textures and then publish a MapUpdateInfo
		// Should only be called after MapFrame is granted to have a parentUiManager
		// (after MapFrame or one of it's ancestors has been added to a UiManager)
		void init();

		void reset();

		void normalizeMap(int n);

		void increaseSeaLevel();
		void decreaseSeaLevel();

		void setLandRenderMode(int mode);
		void setSeaRenderMode(int mode);
		void setLandAndSeaRenderModes(int modeLand, int modeSea);
		
		void publishMapInfo();
};

#endif