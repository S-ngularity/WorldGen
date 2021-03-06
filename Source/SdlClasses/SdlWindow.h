#ifndef SDLWINDOW
#define SDLWINDOW

#include "SdlClasses/SdlTexture.h"

#include "Ui/UiManager.h"

#include <list>
#include <functional>

#include <SDL.h>

class SdlWindow
{
	private:
		// Custom event handler
		std::function<bool(const SDL_Event &e)> evtHandler;

		// Window data
		SDL_Window* window;
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

	protected:
		std::unique_ptr<UiManager> windowUiManager;

		unsigned int windowID;

		void setWindowSdlEvtHandler(std::function<bool(const SDL_Event &e)> eventHandler);

		// returns window's renderer so others can render with it
		SDL_Renderer* getRenderer();

	public:
		SdlWindow(char const *title, int x, int y, int width, int height, int resolutionWidth, int resolutionHeight, Uint32 windowFlags, Uint32 rendererFlags);
		SdlWindow(const SdlWindow&) = delete;
		SdlWindow& operator=(const SdlWindow&) = delete;
		virtual ~SdlWindow();

		bool handleSdlEvent(const SDL_Event &e);

		// Shows windows contents
		void doRefresh();

		void show();
		void hide();

		int getWindowWidth() const;
		int getWindowHeight() const;

		double getWindowWidthScale() const;
		double getWindowHeightScale() const;
		
		// Window focii
		bool hasMouseFocus() const;
		bool hasKeyboardFocus() const;
		bool isMinimized() const;
		bool isShown() const;
};

#endif
