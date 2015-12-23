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
		// Reference to a pointer that keeps the selected map as it's target
		std::shared_ptr<Map> &selectedMap;

		// Generated (fixed) texture from the selected maps's data.
		// It's rendered to the MapFrame's actual texture with applied scrolling/zooming
		std::unique_ptr<MapTexture> mapTexture;

		UiLabel *mouseTooltipPtr;

		bool clickHappenedHere;
		int mouseLastX, mouseLastY;
		double mouseXDragOffset, mouseYDragOffset;
		int mapOffset;

		int zoomX, zoomY, zoomW, zoomH;

		void updateMouseText();
		
		bool mapPosFromMouse(int &x, int &y);

		bool customSdlEvtHandler(SDL_Event &e);

		void preRenderProcedure();

	public:
		MapFrame(int x, int y, int w, int h, std::shared_ptr<Map> &selectedMapPtr);

		// Initializes the textures & other settings and then publish a MapUpdateInfo
		// Should only be called after MapFrame is granted to have a parentUiManager
		// (after MapFrame or one of it's ancestors has been added to a UiManager)
		void init();

		void updateTexture();
		void resetZoom();

		void setLandRenderMode(int mode);
		void setSeaRenderMode(int mode);
		void setLandAndSeaRenderModes(int modeLand, int modeSea);
};

#endif