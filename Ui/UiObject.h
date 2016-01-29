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

		std::function<bool(const SDL_Event &e)> evtHandler;
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

		void getUiObjectOffset(int &xOff, int &yOff) const;
		std::shared_ptr<SdlTexture> getTexture();

	public:
		UiObject(	int xOffset, int yOffset, 
					int width, int height, 
					std::shared_ptr<SdlTexture> texture, 
					std::function<bool(const SDL_Event &e)> eventHandler);
		UiObject(const UiObject&) = delete;
		UiObject& operator=(const UiObject&) = delete;
		virtual ~UiObject();

		// Settings
		void addChild(std::shared_ptr<UiObject> child);
		
		void setUiObjectTexture(std::shared_ptr<SdlTexture> texture);
		
		void setUiObjectOffset(int x, int y); // Offset from parent
		void setUiObjectSize(int width, int height); // Also sets the logical size
		void setUiObjectLogicalSize(int logicalWidth, int logicalHeight);
		
		int getWidth() const;
		int getHeight() const;

		// Render
		void setPreRenderProcedure(std::function<void()> procedure);
		void setPostRenderProcedure(std::function<void()> procedure);
		
		void render(int parentX, int parentY);
		void renderScaled(int parentX, int parentY, double widthScale, double heightScale);

		// Events
		void setSdlEventHandler(std::function<bool(const SDL_Event &e)> eventHandler);

		bool handleSdlEventMouse(const SDL_Event &e);
		bool handleSdlEventKeyboard(const SDL_Event &e);

		// Mouse
		bool isMouseInside() const;
		static bool getRelativeMousePos(const UiObject &obj, int &x, int &y);
};

# endif