#ifndef UIOBJECT
#define UIOBJECT

#include "Ui/UiManager.h"

#include <list>
#include <functional>

#include <SDL2/SDL.h>

class SdlTexture;

class UiObject
{
	private:
		int xOffset, yOffset; // offset from parent's 0, 0

		int width, height;
		double scaleW, scaleH;

		int logicalWidth, logicalHeight;

		SdlTexture *uiTexture;

		std::function<bool(SDL_Event& e)> evtHandler;
		std::function<void()> preRenderProcedure;
		std::function<void()> postRenderProcedure;

		UiObject* parent;
		std::list<UiObject*> childList;

		void bringToFront();


	protected:
		static UiObject *mouseOnTop;

		UiManager *parentUiManager;

		int absoluteX, absoluteY;

	public:
		UiObject(int xOff, int yOff, int w, int h);
		UiObject(int xOff, int yOff, SdlTexture *t, std::function<bool(SDL_Event& e)> evth);
		UiObject(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<bool(SDL_Event& e)> evth);
		virtual ~UiObject();

		void setParentUiManager(UiManager *uiMngr);

		void addChild(UiObject *c);

		// Settings
		void setUiObjectTexture(SdlTexture *t);
		void setUiObjectTextureNoDelete(SdlTexture *t);
		
		void setUiObjectOffset(int x, int y); // Offset from parent
		void setUiObjectSize(int w, int h);

		void setUiObjectLogicalSize(int logicalW, int logicalH);
		
		int getWidth();
		int getHeight();

		// Render
		void setPreRenderProcedure(std::function<void()> procedure);
		void setPostRenderProcedure(std::function<void()> procedure);
		
		void render(int parentX, int parentY);
		void renderScaled(int parentX, int parentY, double sW, double sH);

		// Events
		void setSdlEventHandler(std::function<bool(SDL_Event& e)> evth);

		bool handleSdlEventMouse(SDL_Event& e);
		bool handleSdlEventKeyboard(SDL_Event& e);

		// Mouse
		bool isMouseInside();
		static bool getRelativeMousePos(UiObject *obj, int *x, int *y);
};

# endif