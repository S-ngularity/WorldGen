#ifndef MAPFRAME
#define MAPFRAME

#include "SdlClasses/SdlTexture.h"

#include "SdlClasses/UiObject.h"
#include "SdlClasses/UiObserver.h"
#include "SdlClasses/UiPublisher.h"

#include "SdlClasses/UiCustomObjects/MouseHeightText.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class Noise;
class MapTexture;

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 20

class MapFrame : public UiObject, public UiObserver, public UiPublisher
{
	private:
		MapTexture *mapTexture;

		int numMaps;
		Map* *mapVect;

		int selectedMap;
		int selectedNoise;

		Noise* noiseVect[3];
		//OpenSimplexNoise noiseSimplex;//(map, 10, 0.004, 0.6, 1.9);
		//DiamSqNoise noiseDiam;
		//MyNoise noiseMy;

		MouseHeightText *mouseText;

		bool mapPosFromMouse(int *x, int *y);
		
		void updateMouseText();
		void renderMouseText();

		void runNoise();
		void resetNoise();

		bool handleInternalSdlEvent(SDL_Event &e);
		bool handleUiEvent(int evtId) override;
	public:
		MapFrame(SDL_Renderer *r, int x, int y, int w, int h, Map* mapVect[], int num);
		~MapFrame();
};

#endif