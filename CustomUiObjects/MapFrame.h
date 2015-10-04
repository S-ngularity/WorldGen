#ifndef MAPFRAME
#define MAPFRAME

#include "Ui/UiObject.h"

#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class Noise;
class MapTexture;
class UiManager;
class UiLabel;

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 25

class MapFrame : public UiObject
{
	private:
		std::shared_ptr<MapTexture> mapTexture;

		int numMaps;
		Map* *mapArray;

		int selectedMap;
		int selectedNoise;

		Noise* noiseArray[3];
		//OpenSimplexNoise noiseSimplex;//(map, 10, 0.004, 0.6, 1.9);
		//DiamSqNoise noiseDiam;
		//MyNoise noiseMy;

		UiLabel *mouseTooltip;

		void updateMouseText();
		
		bool mapPosFromMouse(int *x, int *y);
		
		void resetNoise();

		bool customSdlEvtHandler(SDL_Event &e);

		void publishMapInfo();

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