#ifndef NOISEWINDOW
#define NOISEWINDOW

#include "SdlClasses/SdlWindow.h"

#include <memory>
#include <vector>

#include <SDL.h>

class Map;
class Noise;
class MapFrame;
class UiLabel;
class UiCode;

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
		
		std::vector<std::shared_ptr<Map>> mapVector;
		std::shared_ptr<Map> selectedMap;

		std::vector<std::unique_ptr<Noise>> noiseVector;
		int selectedNoiseIdx;

		MapFrame *mapFramePtr;
		UiLabel *mapInfoTextPtr;
		UiLabel *noiseInfoTextPtr;

		int normalizedLevel;
		
		long subscribeTkUiCode;

		void selectMap(int index);
		void selectNoise(int index);

		void runNoise();

		void normalizeMap(int n);

		// Ui Event handlers
		void handleUiCode(const UiCode &c);

		void updateMap();
		void updateMapInfo();

		// UI creation
		void createGui();
		std::shared_ptr<SdlTexture> createDrawnTexture(int width, int height, int r, int g, int b, int a);

		bool customSdlEvtHandler(const SDL_Event &e);

	public:
		NoiseWindow();
		NoiseWindow(const NoiseWindow&) = delete;
		NoiseWindow& operator=(const NoiseWindow&) = delete;
		~NoiseWindow();
};

#endif
