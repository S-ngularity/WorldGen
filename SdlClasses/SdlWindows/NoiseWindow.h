#ifndef NOISEWINDOW
#define NOISEWINDOW


#include "SdlClasses/SdlWindow.h"
#include "SdlClasses/SdlTexture.h"
#include "SdlClasses/SdlTextures/MapTexture.h"

#include "SdlClasses/SdlWindows/WalkWindow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;
class Noise;
class UiObject;

const int SCREEN_WIDTH = 800;//map.getMapWidth();
const int SCREEN_HEIGHT = 800;//map.getMapHeight();

// Simplex noise settings
const int octaves = 10; const double freq = 0.003, persistence = 0.6, freqDiv = 2.2;

// ask for noise screen update at every X percent completed
#define UPDATE_AT_PERCENT 20

class NoiseWindow : public SdlWindow
{
	private:
		int numMaps;
		Map* *mapVect;
		MapTexture mapTexture;

		int selectedMap;
		int selectedNoise;

		Noise* noiseVect[3];
		//OpenSimplexNoise noiseSimplex;//(map, 10, 0.004, 0.6, 1.9);
		//DiamSqNoise noiseDiam;
		//MyNoise noiseMy;

		SdlTexture heightInfoTex;
		TTF_Font *heightInfoFont;

		WalkWindow walkWindow;

		UiObject *gui;

		void createGui();

		// GUI actions
		bool btMapClicked(SDL_Event &e, int i);
		bool btNoiseClicked(SDL_Event &e, int i);

		// this window's specific event handler to be called in the superclass event handler
		void eventHandlerFunc(SDL_Event& e);

		void mapPosFromMouse(int *x, int *y);
		void updateInfoTex();

		void runNoise();
		void resetNoise();

	public:
		NoiseWindow(Map* mapVect[], int num);
		~NoiseWindow();
};

#endif
