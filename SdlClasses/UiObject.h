#ifndef UIOBJECT
#define UIOBJECT

#include <list>
#include <functional>

#include <SDL2/SDL.h>

class SdlTexture;

class UiObject
{
	private:
		int ancientX, ancientY;
		int xOffset, yOffset; // offset from parent's 0, 0
		
		int width, height;
		double scaleW, scaleH;
		double mouseScaleW, mouseScaleH;

		std::list<UiObject*> childList;
		SdlTexture *uiTexture;

		std::function<bool(SDL_Event& e)> evtHandler;

	public:
		UiObject(int xOff, int yOff, int w, int h);
		UiObject(int xOff, int yOff, SdlTexture *t, std::function<bool(SDL_Event& e)> evth);
		UiObject(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<bool(SDL_Event& e)> evth);
		virtual ~UiObject();

		void render(SDL_Renderer *r, int x, int y);
		void renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH);

		bool handleSdlEvent(SDL_Event& e);
		void setSdlEventHandler(std::function<bool(SDL_Event& e)> evth);

		bool isMouseEvtInside(SDL_Event& e);
		void setMouseScale(double sW, double sH);

		void addChild(UiObject *c);
		
		int getWidth();
		int getHeight();

		static void getRelativeMousePos(UiObject *obj, int *x, int *y);
};

# endif