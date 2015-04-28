#ifndef NOISEWINDOW
#define NOISEWINDOW

#include "Map.h"
#include "SdlClasses/SdlWindow.h"
#include "SdlClasses/SdlTexture.h"
#include "SdlClasses/SdlTextures/MapTexture.h"

//#include "Noises/MyNoise.h"
//#include "Noises/DiamSqNoise.h"
#include "Noises/OpenSimplexNoise.h"

#include "SdlClasses/SdlWindows/WalkWindow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 800;//map.getMapWidth();
const int SCREEN_HEIGHT = 800;//map.getMapHeight();

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 20

class NoiseWindow : public SdlWindow
{
	private:
		Map &worldMap;
		MapTexture mapTexture;
		
		OpenSimplexNoise noise;//(map, 10, 0.004, 0.6, 1.9);
		//DiamSqNoise noise(map);
		//MyNoise noise(map);

		SdlTexture heightInfoTex;
		TTF_Font *heightInfoFont;

		WalkWindow walkWindow;

		void mapPosFromMouse(int *x, int *y);
		void updateInfoTex();

	public:
		NoiseWindow(Map &theMap);
		~NoiseWindow();
		
		void handleImplementedEvents(SDL_Event& e);

		void runNoise();
};

#endif
