#ifndef SDLWINDOW
#define SDLWINDOW

#include <functional>

#include <SDL2/SDL.h>

class SdlWindow
{
	private:
		// Custom event handler
		std::function<void(SDL_Event& e)> evtHandler;
		
		// Window data
		SDL_Renderer* renderer;

		// Window dimensions
		int width;
		int height;

		// Original window dimensions
		int originalWidth;
		int originalHeight;

		// Window focus
		bool mouseFocus;
		bool keyboardFocus;
		// bool fullScreen;
		bool minimized;
		bool shown;

	protected:
		SDL_Window* window;
		unsigned int windowID;

		void setEventHandler(std::function<void(SDL_Event& e)> evth);

	public:
		SdlWindow(char const *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags);
		virtual ~SdlWindow();

		void handleEvent(SDL_Event& e);

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

		// Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();
		bool isShown();
};

#endif
