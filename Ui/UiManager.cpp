#include "Ui/UiManager.h"

#include "Ui/UiObject.h"

UiManager::UiManager(SDL_Renderer *r, int w, int h, double wScaleW, double wScaleH) : 
	renderer(r), 
	width(w), 
	height(h),
	windowScaleW(wScaleW),
	windowScaleH(wScaleH),
	focusedUiObject(NULL)
{}

UiManager::~UiManager()
{}


void UiManager::addChild(std::shared_ptr<UiObject> c)
{
	c->setParentUiManager(this);

	childList.push_front(c);
}

int UiManager::getWidth()
{
	return width;
}

int UiManager::getHeight()
{
	return height;
}

void UiManager::setFocusedUiObject(UiObject* obj)
{
	focusedUiObject = obj;
}

UiObject* UiManager::getFocusedUiObject()
{
	return focusedUiObject;
}

SDL_Renderer* UiManager::getRenderer()
{
	return renderer;
}


// ----- Render ----- //

void UiManager::render()
{
	for(auto it = childList.rbegin(); it != childList.rend(); ++it)
	{
		(*it)->render(0, 0);
	}
}


// ----- Events ----- //

bool UiManager::handleSdlEvent(const SDL_Event& e)
{
	bool isMouseEvt = 	e.type == SDL_MOUSEMOTION || 
						e.type == SDL_MOUSEBUTTONDOWN || 
						e.type == SDL_MOUSEBUTTONUP || 
						e.type == SDL_MOUSEWHEEL;

	
	if(isMouseEvt)
	{
		// a mouse evt is always returned as treated by the deepest child
		// if the event happened inside it, because the mouse should never 
		// be treated outside the UiObject where the pointer is, 
		// or else it would be as if the click "passed
		// through" the object and had an effect on the object behind
		
		for(std::shared_ptr<UiObject> childUiObj : childList)
		{
			if(childUiObj->isMouseInside())
			{
				childUiObj->handleSdlEventMouse(e);

				return true;
			}
		}
	}

	else
	{
		if(focusedUiObject != NULL)
			return focusedUiObject->handleSdlEventKeyboard(e);
	}

	return false;
}


// ----- Window ----- //

void UiManager::setWindowScale(double sW, double sH)
{
	windowScaleW = sW;
	windowScaleH = sH;
}

double UiManager::getWindowScaleW()
{
	return windowScaleW;
}

double UiManager::getWindowScaleH()
{
	return windowScaleH;
}
