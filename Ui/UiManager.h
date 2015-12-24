#ifndef UIMANAGER
#define UIMANAGER

#include <list>
#include <memory>

#include <SDL2/SDL.h>

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
		UiManager(SDL_Renderer *r, int w, int h, double wScaleW, double wScaleH);
		UiManager(const UiManager&) = delete;
		UiManager& operator=(const UiManager&) = delete;
		~UiManager();

		void addChild(std::shared_ptr<UiObject> c);

		int getWidth();
		int getHeight();

		void setFocusedUiObject(UiObject* obj);
		UiObject* getFocusedUiObject();
		
		SDL_Renderer* getRenderer();

		// Render
		void render();

		// Events
		bool handleSdlEvent(const SDL_Event& e);

		// Window
		void setWindowScale(double sW, double sH);
		double getWindowScaleW();
		double getWindowScaleH();
};

# endif