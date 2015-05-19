#include "UiObject.h"

UiObject::UiObject(int xOff, int yOff, SDL_Texture *t, int w, int h, std::function<bool(SDL_Event& e)> evth) : 
	evtHandler(evth)
{
	ancientX = xOff;
	ancientY = yOff;
	xOffset = xOff;
	yOffset = yOff;
	
	uiTexture = t;
	
	width = w;
	height = h;
	scaleW = 1;
	scaleH = 1;
}

UiObject::~UiObject()
{
	for(UiObject *childUiObj : childList)
		delete childUiObj;

	SDL_DestroyTexture(uiTexture);
}

void UiObject::render(SDL_Renderer *r, int x, int y) // parent's x & y
{
	ancientX = x + xOffset;
	ancientY = y + yOffset;

	scaleW = 1;
	scaleH = 1;

	if(uiTexture != NULL)
	{
		SDL_Rect renderRect = {ancientX, ancientY, width, height};
		SDL_RenderCopy(r, uiTexture, NULL, &renderRect);
	}

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
	{
		SDL_Rect renderRect = {(int)(ancientX * scaleW), (int)(ancientY * scaleH), (int)(width * scaleW), (int)(height * scaleH)};
		SDL_RenderCopy(r, uiTexture, NULL, &renderRect);
	}

	for(UiObject *childUiObj : childList)
	{
		childUiObj->renderScaled(r, ancientX, ancientY, scaleW, scaleH);
	}
}

bool UiObject::handleEvent(SDL_Event& e)
{
	// é pra: se for keyboard evt, passa pro focus ou trata aqui
	// se for mouse evt, itera pelos filhos e passa evt pro que estiver em baixo do mouse
	for(UiObject *childUiObj : childList)
	{
		if(childUiObj->isMouseEvtInside(e))
		{
			// if child handled the evt, evt is handled
			if(childUiObj->handleEvent(e) == true)
				return true;
			
			else
				break;
		}
	}

	bool isMouseEvt = 	e.type == SDL_MOUSEMOTION || 
						e.type == SDL_MOUSEBUTTONDOWN || 
						e.type == SDL_MOUSEBUTTONUP || 
						e.type == SDL_MOUSEWHEEL;

	// if child didn't handle the evt, try to handle it with my handler
	if(isMouseEvtInside(e)) // handle keyboard & mouse only if it's inside this object
	{
		// a mouse evt is always returned as treated (because the mouse should 
		// never be treated outside the UiObject where the pointer is)
		if(evtHandler != NULL)
			return evtHandler(e) || isMouseEvt;
		else
			return isMouseEvt;
	}
	
	return false;
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

void UiObject::addChild(UiObject *c)
{
	childList.push_front(c);
}
