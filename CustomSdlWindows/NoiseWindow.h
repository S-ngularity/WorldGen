#ifndef NOISEWINDOW
#define NOISEWINDOW

#include "SdlClasses/SdlWindow.h"

#include <memory>

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
		void customUiEventHandler(UiEventCode &c);

		// UI creation
		void createGui();
		std::shared_ptr<SdlTexture> createDrawnTexture(int width, int height, int r, int g, int b, int a);

	public:
		NoiseWindow(Map* mapArr[], int num);
		~NoiseWindow();
};

#endif
