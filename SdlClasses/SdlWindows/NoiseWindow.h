#ifndef NOISEWINDOW
#define NOISEWINDOW

#include "SdlClasses/SdlWindow.h"

#include <SDL2/SDL.h>

class Map;
class Noise;
class UiObject;
class UiEventCode;

const int SIDEBAR_WIDTH = 190;
const int SCREEN_WIDTH = 800 + SIDEBAR_WIDTH;//map.getMapWidth();
const int SCREEN_HEIGHT = 800;//map.getMapHeight();

class NoiseWindow : public SdlWindow
{
	private:
		int numMaps;
		Map* *mapArray;

		// Ui Event handlers
		void contentsChanged(UiEventCode &c);

		// UI creation
		void createGui();
		SDL_Texture* createDrawnTexture(int width, int height, int r, int g, int b, int a);

		// UI actions
		bool btMapClicked(SDL_Event &e, int i);
		bool btNoiseClicked(SDL_Event &e, int i);

	public:
		NoiseWindow(Map* mapArr[], int num);
		~NoiseWindow();
};

#endif
