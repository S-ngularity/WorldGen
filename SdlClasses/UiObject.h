#ifndef UIOBJECT
#define UIOBJECT

#include <list>
#include <functional>

#include <SDL2/SDL.h>

class SdlEventHandler;

class UiObject
{
	private:
		int ancientX, ancientY;
		int xOffset, yOffset; // offset from parent's 0, 0
		
		int width, height;
		double scaleW, scaleH;

		std::list<UiObject*> childList;
		SDL_Texture *uiTexture;

		std::function<bool(SDL_Event& e)> evtHandler;

	public:
		UiObject(int xOff, int yOff, SDL_Texture *t, int w, int h, std::function<bool(SDL_Event& e)> evth);
		~UiObject();

		void render(SDL_Renderer *r, int x, int y);
		void renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH);

		bool handleEvent(SDL_Event& e);
		bool isMouseEvtInside(SDL_Event& e);

		void addChild(UiObject *c);
};

# endif