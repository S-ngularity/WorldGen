#ifndef NOISEWINDOW
#define NOISEWINDOW

#include "SdlClasses/SdlWindow.h"

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

class Map;
class Noise;
class UiObject;
class MapFrame;
class UiLabel;
class UiCode;
class MapInfoUpdate;

// ask for noise screen update at every X percent completed
const int UPDATE_AT_PERCENT = 25;

const int SIDEBAR_WIDTH = 210;
const int SCREEN_WIDTH = 800 + SIDEBAR_WIDTH;//map.getMapWidth();
const int SCREEN_HEIGHT = 800;//map.getMapHeight();

class NoiseWindow : public SdlWindow
{
	private:
		// Simplex noise settings
		const int octaves = 10; const double freq = 0.0024, persistence = 0.6, freqDiv = 2.08;
		//const int octaves = 10; const double freq = 0.0028, persistence = 0.6, freqDiv = 1.9;
		
		std::vector<Map> mapVector;
		Map *selectedMap;

		int normalizedLevel;

		int selectedNoiseId;
		std::unique_ptr<Noise> selectedNoise;

		UiLabel *mapInfoText;
		UiLabel *noiseInfoText;

		MapFrame *mapFrame;

		long subscribeTkUiCode, subscribeTkMapInfoUpdate;

		void selectNoise(int id);
		void runNoise();

		// Ui Event handlers
		void customUiEventHandler(UiCode &c);
		void updateMapInfo(MapInfoUpdate &info);

		// UI creation
		void createGui();
		std::shared_ptr<SdlTexture> createDrawnTexture(int width, int height, int r, int g, int b, int a);

		bool customSdlEvtHandler(SDL_Event &e);

	public:
		NoiseWindow();
		~NoiseWindow();
};

#endif
