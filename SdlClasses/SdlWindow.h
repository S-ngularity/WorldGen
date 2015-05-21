#ifndef SDLWINDOW
#define SDLWINDOW

#include "SdlClasses/UiObject.h"

#include <functional>

#include <SDL2/SDL.h>

class SdlWindow
{
	private:
		// Custom event handler
		std::function<void(SDL_Event& e)> sdlEvtHandler;
		
		// Window data
		SDL_Renderer* renderer;

		// Window dimensions
		int width;
		int height;

		// Original window dimensions
		int originalWidth;
		int originalHeight;

		bool windowSizeChanged;

		// Window focus
		bool mouseFocus;
		bool keyboardFocus;
		// bool fullScreen;
		bool minimized;
		bool shown;

		bool askingForRefresh;

		void doRefreshIfAsked();

	protected:
		UiObject gui;

		SDL_Window* window;
		unsigned int windowID;

		void setWindowSdlEvtHandler(std::function<bool(SDL_Event& e)> evth);

	public:
		SdlWindow(char const *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags);
		virtual ~SdlWindow();

		bool handleSdlEvent(SDL_Event& e);

		// returns window's renderer so others can render onto it
		SDL_Renderer* getRenderer();

		void show();
		void hide();

		// Shows windows contents
		void refresh();

		int getWindowWidth();
		int getWindowHeight();

		double getWindowWidthScale();
		double getWindowHeightScale();

		bool hasWindowSizeChanged();
		
		// Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();
		bool isShown();
};

#endif
