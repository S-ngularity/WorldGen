#ifndef SDLWINDOW
#define SDLWINDOW

#include <SDL2/SDL.h>

//Total windows
#define TOTAL_WINDOWS = 3;

class SdlWindow
{
	public:
		SdlWindow(char* title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags);
		~SdlWindow();

		virtual int init() = 0;

		void handleEvent(SDL_Event& e);

		//Focuses on window
		void focus();

		//Shows windows contents
		void render();

		//Deallocates internals
		void free();

		int getWidth();
		int getHeight();

		//Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();
		bool isShown();

	protected:
		virtual void handleImplementedEvents(SDL_Event& e) = 0;

	private:
		//Window data
		SDL_Window* window;
		SDL_Renderer* renderer;
		unsigned int windowID;

		//Window dimensions
		int width;
		int height;

		//Window focus
		bool mouseFocus;
		bool keyboardFocus;
		//bool fullScreen;
		bool minimized;
		bool shown;
};

#endif
