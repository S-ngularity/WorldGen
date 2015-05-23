#ifndef NOISEWINDOW
#define NOISEWINDOW


#include "SdlClasses/SdlWindow.h"

#include "SdlClasses/SdlWindows/WalkWindow.h"

#include "SdlClasses/UiObserver.h"
#include "SdlClasses/UiPublisher.h"

#include <SDL2/SDL.h>

class Map;
class Noise;
class UiObject;

const int SIDEBAR_WIDTH = 190;
const int SCREEN_WIDTH = 800 + SIDEBAR_WIDTH;//map.getMapWidth();
const int SCREEN_HEIGHT = 800;//map.getMapHeight();

class NoiseWindow : public SdlWindow, public UiObserver, public UiPublisher
{
	private:
		int numMaps;
		Map* *mapVect;

		WalkWindow walkWindow;

		void createGui();
		SDL_Texture* createDrawnTexture(int width, int height, int r, int g, int b, int a);

		// GUI actions
		bool btMapClicked(SDL_Event &e, int i);
		bool btNoiseClicked(SDL_Event &e, int i);

		// this window's specific sdl event handler to be called in the superclass event handler
		bool handleInternalSdlEvent(SDL_Event& e);
		bool handleUiEvent(int evtId) override;

	public:
		NoiseWindow(Map* mapVect[], int num);
		~NoiseWindow();
};

#endif
