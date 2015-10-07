#ifndef MAPFRAME
#define MAPFRAME

#include "Ui/UiObject.h"

#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class Noise;
class MapTexture;
class SdlTexture;
class UiManager;
class UiLabel;

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 25

class MapFrame : public UiObject
{
	private:
		std::unique_ptr<MapTexture> mapTexture;
		std::unique_ptr<SdlTexture> *frameTexture;

		int numMaps;
		Map* *mapArray;

		static const int numNoises = 2;
		Noise* noiseArray[numNoises];

		int selectedMap;
		int selectedNoise;

		UiLabel *mouseTooltip;

		bool clickHappenedHere;
		int mouseLastX, mouseLastY;
		double mouseXDragOffset, mouseYDragOffset;
		int mapOffset;

		int zoomX, zoomY, zoomW, zoomH;

		void updateMouseText();
		
		bool mapPosFromMouse(int *x, int *y);

		bool customSdlEvtHandler(SDL_Event &e);

		void publishMapInfo();

		void preRenderProcedure();

	public:
		MapFrame(UiManager *parentUiMngr, int x, int y, int w, int h, Map* mapArr[], int num);
		~MapFrame();

		void runNoise();

		void selectMap(int i);
		void selectNoise(int i);

		void normalizeMap(int n);

		void increaseSeaLevel();
		void decreaseSeaLevel();

		void setLandRenderMode(int mode);
		void setSeaRenderMode(int mode);
		void setLandAndSeaRenderModes(int modeLand, int modeSea);
};

#endif