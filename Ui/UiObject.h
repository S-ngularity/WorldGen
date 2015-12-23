#ifndef UIOBJECT
#define UIOBJECT

#include "Ui/UiManager.h"

#include <list>
#include <functional>
#include <memory>

#include <SDL2/SDL.h>

class SdlTexture;

class UiObject : public std::enable_shared_from_this<UiObject>
{
	private:
		int xOffset, yOffset; // offset from parent's 0, 0

		int width, height;
		double scaleW, scaleH;

		int logicalWidth, logicalHeight;

		std::shared_ptr<SdlTexture> uiTexture;

		std::function<bool(SDL_Event& e)> evtHandler;
		std::function<void()> preRenderProcedure;
		std::function<void()> postRenderProcedure;

		UiObject* parent;
		std::list<std::shared_ptr<UiObject>> childList;

		void bringToFront();

		// Is automatically called when this UiObject or one of it's parents is added to a UiManager
		// Until then, the parentUiManager is NULL and many functions that rely on it will fail
		void setParentUiManager(UiManager *uiMngr);
		friend class UiManager;

	protected:
		static UiObject *mouseOnTop;

		UiManager *parentUiManager;

		int absoluteX, absoluteY;

		void getUiObjectOffset(int &xOff, int &yOff);
		std::shared_ptr<SdlTexture> getTexture();

	public:
		UiObject(int xOff, int yOff, int w, int h);
		UiObject(int xOff, int yOff, std::shared_ptr<SdlTexture> t, std::function<bool(SDL_Event& e)> evth);
		UiObject(int xOff, int yOff, int w, int h, std::shared_ptr<SdlTexture> t, std::function<bool(SDL_Event& e)> evth);
		virtual ~UiObject();

		// Settings
		void addChild(std::shared_ptr<UiObject> c);
		
		void setUiObjectTexture(std::shared_ptr<SdlTexture> t);
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
		static bool getRelativeMousePos(UiObject &obj, int &x, int &y);
};

# endif