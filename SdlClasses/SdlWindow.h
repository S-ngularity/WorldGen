#ifndef SDLWINDOW
#define SDLWINDOW

#include "SdlClasses/SdlTexture.h"

#include "Ui/UiObject.h"

#include <list>
#include <functional>

#include <SDL2/SDL.h>

class SdlWindow
{
	private:
		// Custom event handler
		std::function<bool(SDL_Event& e)> evtHandler;

		// Window data
		SDL_Renderer *wndRenderer;

		SdlTexture resolutionTexture;

		// Resolution independent from window size
		int resolutionWidth;
		int resolutionHeight;

		// Window dimensions
		int windowWidth;
		int windowHeight;

		// Original window dimensions
		int originalWndWidth;
		int originalWndHeight;

		// Window focus
		bool mouseFocus;
		bool keyboardFocus;
		//bool fullScreen;
		bool minimized;
		bool shown;

		// Refreshing flag
		bool refreshSignaled;

	protected:
		UiObject *gui;

		SDL_Window* window;
		unsigned int windowID;

		void setWindowSdlEvtHandler(std::function<bool(SDL_Event& e)> evth);

		// returns window's renderer so others can render onto it
		SDL_Renderer* getRenderer();

	public:
		SdlWindow(char const *title, int x, int y, int w, int h, int resW, int resH, Uint32 windowFlags, Uint32 rendererFlags);
		virtual ~SdlWindow();

		bool handleSdlEvent(SDL_Event& e);

		void show();
		void hide();

		// Shows windows contents
		void signalRefresh();
		void doRefresh();

		int getWindowWidth();
		int getWindowHeight();

		double getWindowWidthScale();
		double getWindowHeightScale();
		
		// Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();
		bool isShown();
};

#endif
