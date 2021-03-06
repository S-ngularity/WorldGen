#include "Ui/UiManager.h"

#include "Ui/UiObject.h"

UiManager::UiManager(SDL_Renderer *r, int width, int height, double windowWidthScale, double windowHeightScale) : 
	renderer(r), 
	width(width), 
	height(height),
	windowScaleW(windowWidthScale),
	windowScaleH(windowHeightScale),
	focusedUiObject(NULL)
{}

UiManager::~UiManager()
{}


void UiManager::addChild(std::shared_ptr<UiObject> child)
{
	child->setParentUiManager(this);

	childList.push_front(child);
}

int UiManager::getWidth() const
{
	return width;
}

int UiManager::getHeight() const
{
	return height;
}

void UiManager::setFocusedUiObject(UiObject* obj)
{
	focusedUiObject = obj;
}

const UiObject* UiManager::getFocusedUiObject() const
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

bool UiManager::handleSdlEvent(const SDL_Event &e)
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

void UiManager::setWindowScale(double windowWidthScale, double windowHeightScale)
{
	windowScaleW = windowWidthScale;
	windowScaleH = windowHeightScale;
}

double UiManager::getWindowScaleW() const
{
	return windowScaleW;
}

double UiManager::getWindowScaleH() const
{
	return windowScaleH;
}
