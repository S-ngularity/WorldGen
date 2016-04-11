#ifndef UIMANAGER
#define UIMANAGER

#include <list>
#include <memory>

#include <SDL.h>

class UiObject;


class UiManager
{
	friend class UiObject;
	
	private:
		SDL_Renderer *renderer;
		
		int width, height;
		double windowScaleW, windowScaleH;

		std::list<std::shared_ptr<UiObject>> childList;

		UiObject* focusedUiObject;

	public:
		UiManager(SDL_Renderer *r, int width, int height, double windowWidthScale, double windowHeightScale);
		UiManager(const UiManager&) = delete;
		UiManager& operator=(const UiManager&) = delete;
		~UiManager();

		void addChild(std::shared_ptr<UiObject> child);

		int getWidth() const;
		int getHeight() const;

		void setFocusedUiObject(UiObject* obj);
		const UiObject* getFocusedUiObject() const;
		
		SDL_Renderer* getRenderer();

		// Render
		void render();

		// Events
		bool handleSdlEvent(const SDL_Event &e);

		// Window
		void setWindowScale(double windowWidthScale, double windowHeightScale);
		double getWindowScaleW() const;
		double getWindowScaleH() const;
};

# endif