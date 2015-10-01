#include "Ui/UiManager.h"

#include "Ui/UiObject.h"

UiManager::UiManager(SDL_Renderer *r, int w, int h, double wScaleW, double wScaleH) : 
	renderer(r), 
	width(w), 
	height(h),
	windowScaleW(wScaleW),
	windowScaleH(wScaleH)
{}

UiManager::~UiManager()
{
	for(UiObject *childUiObj : childList)
		delete childUiObj;
}


void UiManager::addChild(UiObject *c)
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

bool UiManager::handleSdlEvent(SDL_Event& e)
{
	bool isMouseEvt = 	e.type == SDL_MOUSEMOTION || 
						e.type == SDL_MOUSEBUTTONDOWN || 
						e.type == SDL_MOUSEBUTTONUP || 
						e.type == SDL_MOUSEWHEEL;

	for(UiObject *childUiObj : childList)
	{
		if(isMouseEvt)
		{
			// a mouse evt is always returned as treated by the deepest child
			// if the event happened inside it, because the mouse should never 
			// be treated outside the UiObject where the pointer is, 
			// or else it would be as if the click "passed
			// through" the object and had an effect on the object behind
			
			if(childUiObj->isMouseInside())
			{
				childUiObj->handleSdlEvent(e);

				return true;
			}
		}

		else
		{
			// should be "isChildSelected/Focused"
			if(childUiObj->isMouseInside())
			{
				// handle keyboard with child's handler or else
				// redirect the keyboard event to parent object's handler
				// because keyboard events are "global" and not specific
				// to where the mouse pointer is
				
				if(childUiObj->handleSdlEvent(e) == true)
						return true;
				else
					// if the execution is here, it won't happen again for 
					// any other child
					break;
			}
		}
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
