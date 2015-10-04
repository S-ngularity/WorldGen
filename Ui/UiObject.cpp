#include "Ui/UiObject.h"

#include "SdlClasses/SdlTexture.h"

#include <iostream>

UiObject *UiObject::mouseOnTop = NULL;

UiObject::UiObject(int xOff, int yOff, int w, int h) : 
	UiObject(xOff, yOff, w, h, NULL, nullptr)
{}

UiObject::UiObject(int xOff, int yOff, std::shared_ptr<SdlTexture> t, std::function<bool(SDL_Event& e)> evth) : 
	UiObject(xOff, yOff, 0, 0, t, evth)
{
	if(t != NULL)
	{
		setUiObjectSize(t->getW(), t->getH());
		setUiObjectLogicalSize(t->getW(), t->getH());
	}
}

UiObject::UiObject(int xOff, int yOff, int w, int h, std::shared_ptr<SdlTexture> t, std::function<bool(SDL_Event& e)> evth) : 
	uiTexture(t),
	evtHandler(evth),
	preRenderProcedure(nullptr),
	postRenderProcedure(nullptr)
{
	parentUiManager = NULL;
	parent = NULL;

	absoluteX = 0;
	absoluteY = 0;
	xOffset = xOff;
	yOffset = yOff;

	width = w;
	height = h;
	scaleW = 1;
	scaleH = 1;

	logicalWidth = width;
	logicalHeight = height;
	
	//uiTexture = t;
}

UiObject::~UiObject()
{
	for(UiObject *childUiObj : childList)
		delete childUiObj;

	//if(uiTexture != NULL)
	//	delete uiTexture;
}


void UiObject::addChild(UiObject *c)
{
	if(c != NULL)
	{
		c->setParentUiManager(parentUiManager);
		c->parent = this;

		childList.push_front(c);
	}
}

void UiObject::bringToFront()
{
	if(parent != NULL)
	{
		if(parent->childList.front() != this)
		{
			parent->childList.remove(this);
			parent->childList.push_front(this);
		}

		parent->bringToFront();
	}

	else
	{
		if(parentUiManager->childList.front() != this)
		{
			parentUiManager->childList.remove(this);
			parentUiManager->childList.push_front(this);
		}
	}
}

// ----- Settings ----- //

void UiObject::setUiObjectTexture(std::shared_ptr<SdlTexture> t)
{
	uiTexture = t;
}

void UiObject::setUiObjectOffset(int x, int y)
{
	xOffset = x;
	yOffset = y;
}

void UiObject::setUiObjectSize(int w, int h)
{
	width = w;
	height = h;
	logicalWidth = w;
	logicalHeight = h;
}

void UiObject::setUiObjectLogicalSize(int logicalW, int logicalH)
{
	logicalWidth = logicalW;
	logicalHeight = logicalH;
}

int UiObject::getWidth()
{
	return width;
}

int UiObject::getHeight()
{
	return height;
}

void UiObject::setParentUiManager(UiManager *uiMngr)
{
	parentUiManager = uiMngr;

	for(UiObject *childUiObj : childList)
		childUiObj->setParentUiManager(uiMngr);
}


// ----- Render ----- //

void UiObject::setPreRenderProcedure(std::function<void()> procedure)
{
	preRenderProcedure = procedure;
}

void UiObject::setPostRenderProcedure(std::function<void()> procedure)
{
	postRenderProcedure = procedure;
}

void UiObject::render(int parentX, int parentY)
{
	absoluteX = parentX + xOffset;
	absoluteY = parentY + yOffset;

	scaleW = 1;
	scaleH = 1;
	
	if(preRenderProcedure)
		preRenderProcedure();

	if(uiTexture != NULL)
	{
		if(parentUiManager == NULL)
			std::cout << "UiObject without parentUiManager (is NULL)." << std::endl;

		uiTexture->renderFitToArea(parentUiManager->getRenderer(), absoluteX, absoluteY, width, height);
	}

	if(postRenderProcedure)
		postRenderProcedure();

	for(auto it = childList.rbegin(); it != childList.rend(); ++it)
	{
		(*it)->render(absoluteX, absoluteY);
	}
}

void UiObject::renderScaled(int parentX, int parentY, double sW, double sH)
{
	absoluteX = parentX + xOffset;
	absoluteY = parentY + yOffset;

	scaleW = sW;
	scaleH = sH;

	if(preRenderProcedure)
		preRenderProcedure();

	if(uiTexture != NULL)
	{
		if(parentUiManager == NULL)
			std::cout << "UiObject without parentUiManager (is NULL)." << std::endl;

		uiTexture->renderFitToArea(parentUiManager->getRenderer(), absoluteX, absoluteY, width * scaleW, height * scaleH);
	}

	if(postRenderProcedure)
		postRenderProcedure();

	for(auto it = childList.rbegin(); it != childList.rend(); ++it)
	{
		(*it)->renderScaled(absoluteX, absoluteY, scaleW, scaleH);
	}
}

// ----- Event ----- //

void UiObject::setSdlEventHandler(std::function<bool(SDL_Event& e)> evth)
{
	evtHandler = evth;
}

bool UiObject::handleSdlEventMouse(SDL_Event& e)
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
		
		for(UiObject *childUiObj : childList)
		{
			if(childUiObj->isMouseInside())
			{
				childUiObj->handleSdlEventMouse(e);

				return true;
			}
		}

		// if mouse wasn't inside any of my child's areas, it's on me
		mouseOnTop = this;

		// therefore handle with my handler. always return mouse event as handled
		if(evtHandler)
			evtHandler(e);

		if(e.type == SDL_MOUSEBUTTONUP)
		{
			parentUiManager->setFocusedUiObject(this);

			bringToFront();
		}

		return true;
	}

	return false;
}

bool UiObject::handleSdlEventKeyboard(SDL_Event& e)
{
	bool isMouseEvt = 	e.type == SDL_MOUSEMOTION || 
						e.type == SDL_MOUSEBUTTONDOWN || 
						e.type == SDL_MOUSEBUTTONUP || 
						e.type == SDL_MOUSEWHEEL;

	if(!isMouseEvt)
	{
		// handle keyboard with child's handler or else
		// redirect the keyboard event to parent object's handler
		// because keyboard events are "global" and not specific
		// to where the mouse pointer is

		bool handled = false;

		if(evtHandler)
			handled = evtHandler(e);

		if(parent != NULL && !handled)
			handled = parent->handleSdlEventKeyboard(e);

		return handled;
	}

	return false;
}

// ----- Mouse ----- //

bool UiObject::isMouseInside()
{
	int x, y;

	SDL_GetMouseState(&x, &y);

	if(parentUiManager == NULL)
			std::cout << "UiObject without parentUiManager (is NULL)." << std::endl;

	if(	x >= absoluteX * parentUiManager->getWindowScaleW() && 
		x < (absoluteX + logicalWidth) * parentUiManager->getWindowScaleW() &&
		y >= absoluteY * parentUiManager->getWindowScaleH() && 
		y < (absoluteY + logicalHeight) * parentUiManager->getWindowScaleH())
	{
		return true;
	}

	else
		return false;
}

bool UiObject::getRelativeMousePos(UiObject *obj, int *x, int *y)
{
	SDL_GetMouseState(x, y);

	if(obj->parentUiManager == NULL)
			std::cout << "UiObject without parentUiManager (is NULL)." << std::endl;

	*x = (*x - obj->absoluteX * obj->parentUiManager->getWindowScaleW()) / obj->parentUiManager->getWindowScaleW();
	*y = (*y - obj->absoluteY * obj->parentUiManager->getWindowScaleH()) / obj->parentUiManager->getWindowScaleH();

	if(	*x < 0 || *x > obj->width || 
		*y < 0 || *y > obj->height)
	{
		*x = -1;
		*y = -1;

		return false;
	}

	else
		return true;
}
