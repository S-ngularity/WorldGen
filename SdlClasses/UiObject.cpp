#include "UiObject.h"

#include "SdlClasses/SdlTexture.h"

UiObject::UiObject(int xOff, int yOff, int w, int h) : 
	UiObject(xOff, yOff, w, h, NULL, nullptr)
{}

UiObject::UiObject(int xOff, int yOff, int w, int h, std::function<bool(SDL_Event& e)> evth) : 
	UiObject(xOff, yOff, w, h, NULL, evth)
{}

UiObject::UiObject(int xOff, int yOff, SdlTexture *t) : 
	UiObject(xOff, yOff, t->getW(), t->getH(), t, nullptr)
{}

UiObject::UiObject(int xOff, int yOff, SdlTexture *t, std::function<bool(SDL_Event& e)> evth) : 
	UiObject(xOff, yOff, t->getW(), t->getH(), t, evth)
{}


UiObject::UiObject(int xOff, int yOff, int w, int h, SdlTexture *t) :
	UiObject(xOff, yOff, w, h, t, nullptr)
{}

UiObject::UiObject(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<bool(SDL_Event& e)> evth) : 
	evtHandler(evth)
{
	ancientX = xOff;
	ancientY = yOff;
	xOffset = xOff;
	yOffset = yOff;
	width = w;
	height = h;
	
	uiTexture = t;

	scaleW = 1;
	scaleH = 1;
}

UiObject::~UiObject()
{
	for(UiObject *childUiObj : childList)
		delete childUiObj;

	if(uiTexture != NULL)
		delete uiTexture;
}

void UiObject::render(SDL_Renderer *r, int x, int y) // parent's x & y
{
	ancientX = x + xOffset;
	ancientY = y + yOffset;

	scaleW = 1;
	scaleH = 1;

	if(uiTexture != NULL)
		uiTexture->renderFitToArea(r, ancientX, ancientY, width, height);

	for(UiObject *childUiObj : childList)
	{
		childUiObj->render(r, ancientX, ancientY);
	}
}

void UiObject::renderScaled(SDL_Renderer *r, int x, int y, double sW, double sH)
{
	ancientX = x + xOffset;
	ancientY = y + yOffset;

	scaleW = sW;
	scaleH = sH;

	if(uiTexture != NULL)
		uiTexture->renderFitToArea(r, ancientX * scaleW, ancientY * scaleH, width * scaleW, height * scaleH);

	for(UiObject *childUiObj : childList)
	{
		childUiObj->renderScaled(r, ancientX, ancientY, scaleW, scaleH);
	}
}

bool UiObject::handleEvent(SDL_Event& e)
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
			
			if(childUiObj->isMouseEvtInside(e))
			{
				childUiObj->handleEvent(e);

				return true;
			}
		}

		else
		{
			// should be "isChildSelected/Focused"
			if(childUiObj->isMouseEvtInside(e))
			{
				// handle keyboard with child's handler or else
				// redirect the keyboard event to parent object's handler
				// because keyboard events are "global" and not specific
				// to where the mouse pointer is
				
				if(childUiObj->handleEvent(e) == true)
						return true;
				else
					// if the execution is here, it won't happen again for 
					// any other child
					break;
			}
		}
	}

	// if child didn't handle the evt or there wasn't a child to handle it,
	// then try to handle it with my handler
	if(isMouseEvt)
	{
		// if mouse wasn't inside any of my child's areas handle the
		// mouse with my handler. always return mouse event as handled
		if(evtHandler)
			evtHandler(e);

		return true;
	}

	else
	{
		// if there was a keyboard event for me, try to handle with my 
		// handler. if it was still not handled, my parent should try
		// to handle it
		if(evtHandler)
			return evtHandler(e);
		else
			return false;
	}
}

bool UiObject::isMouseEvtInside(SDL_Event& e)
{
	int x, y;

	SDL_GetMouseState(&x, &y);

	if(	x >= ancientX * scaleW && 
		x < (ancientX + width) * scaleW &&
		y >= ancientY * scaleH && 
		y < (ancientY + height) * scaleH )
	{
		return true;
	}

	else
		return false;
}

void UiObject::setEventHandler(std::function<bool(SDL_Event& e)> evth)
{
	evtHandler = evth;
}

void UiObject::addChild(UiObject *c)
{
	childList.push_front(c);
}

int UiObject::getWidth()
{
	return width;
}

int UiObject::getHeight()
{
	return height;
}

void UiObject::getRelativeMousePos(UiObject *obj, int *x, int *y)
{
	*x = *x - obj->ancientX * obj->scaleW;
	*y = *y - obj->ancientY * obj->scaleH;
}
