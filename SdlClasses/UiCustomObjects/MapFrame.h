#ifndef MAPFRAME
#define MAPFRAME

#include "SdlClasses/SdlTexture.h"

#include "SdlClasses/UiObject.h"
#include "SdlClasses/UiObserver.h"
#include "SdlClasses/UiPublisher.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class Noise;
class MapTexture;
class UiEventAggregator;

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 20

class MapFrame : public UiObject, public UiObserver, public UiPublisher
{
	private:
		MapTexture *const mapTexture;

		SdlTexture heightInfoTex;
		TTF_Font *heightInfoFont;

		int numMaps;
		Map* *mapVect;

		int selectedMap;
		int selectedNoise;

		Noise* noiseVect[3];
		//OpenSimplexNoise noiseSimplex;//(map, 10, 0.004, 0.6, 1.9);
		//DiamSqNoise noiseDiam;
		//MyNoise noiseMy;

		void mapPosFromMouse(int *x, int *y);
		void updateInfoTex();

		void runNoise();
		void resetNoise();

		bool handleInternalSdlEvent(SDL_Event &e);
		bool handleUiEvent(int evtId) override;
	public:
		MapFrame(int x, int y, int w, int h, MapTexture *mapTex, Map* mapVect[], int num, UiEventAggregator *evtAggr);
		~MapFrame();
};

#endif