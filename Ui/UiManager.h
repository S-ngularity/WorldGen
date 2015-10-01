#ifndef UIMANAGER
#define UIMANAGER

#include <list>

#include <SDL2/SDL.h>

class UiObject;

class UiManager
{
	private:
		SDL_Renderer *renderer;
		
		int width, height;
		double windowScaleW, windowScaleH;

		std::list<UiObject*> childList;

	public:
		UiManager(SDL_Renderer *r, int w, int h, double wScaleW, double wScaleH);
		~UiManager();

		void addChild(UiObject *c);

		int getWidth();
		int getHeight();

		SDL_Renderer* getRenderer();

		// Render
		void render();

		// Events
		bool handleSdlEvent(SDL_Event& e);

		// Window
		void setWindowScale(double sW, double sH);
		double getWindowScaleW();
		double getWindowScaleH();
};

# endif