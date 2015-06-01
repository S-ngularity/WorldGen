#ifndef MAPFRAME
#define MAPFRAME

#include "SdlClasses/SdlTexture.h"

#include "Ui/UiObject.h"

#include "Ui/UiCustomObjects/MouseHeightText.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class Noise;
class MapTexture;

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 25

class MapFrame : public UiObject
{
	private:
		MapTexture *mapTexture;

		int numMaps;
		Map* *mapArray;

		int selectedMap;
		int selectedNoise;

		Noise* noiseArray[3];
		//OpenSimplexNoise noiseSimplex;//(map, 10, 0.004, 0.6, 1.9);
		//DiamSqNoise noiseDiam;
		//MyNoise noiseMy;

		MouseHeightText *mouseText;

		void runNoise();
		void resetNoise();

		void updateMouseText();
		
		bool mapPosFromMouse(int *x, int *y);

		bool handleInternalSdlEvent(SDL_Event &e);

	public:
		MapFrame(SDL_Renderer *r, int x, int y, int w, int h, Map* mapArr[], int num);
		~MapFrame();

		void selectMap(int i);
		void selectNoise(int i);

		void normalizeMap();

		void increaseSeaLevel();
		void decreaseSeaLevel();

		void setLandRenderMode(int mode);
		void setSeaRenderMode(int mode);
		void setLandAndSeaRenderModes(int modeLand, int modeSea);
};

#endif