#include "Ui/UiObject.h"

#include "SdlClasses/SdlTexture.h"

#include <iostream>

UiObject *UiObject::mouseOnTop = NULL;

UiObject::UiObject(int xOffset, int yOffset, int width, int height, std::shared_ptr<SdlTexture> texture, std::function<bool(const SDL_Event &e)> eventHandler) : 
	uiTexture(texture),
	evtHandler(eventHandler),
	preRenderProcedure(nullptr),
	postRenderProcedure(nullptr)
{
	parentUiManager = NULL;
	parent = NULL;

	absoluteX = 0;
	absoluteY = 0;
	this->xOffset = xOffset;
	this->yOffset = yOffset;

	scaleW = 1;
	scaleH = 1;
	this->width = width;
	this->height = height;

	logicalWidth = this->width;
	logicalHeight = this->height;
}

UiObject::~UiObject()
{}


// ----- Settings ----- //

void UiObject::addChild(std::shared_ptr<UiObject> child)
{
	if(child != NULL)
	{
		child->setParentUiManager(parentUiManager);
		child->parent = this;

		childList.push_front(child);
	}
}

void UiObject::bringToFront()
{
	if(parent != NULL)
	{
		if(parent->childList.front() != shared_from_this())
		{
			parent->childList.remove(shared_from_this());
			parent->childList.push_front(shared_from_this());
		}

		parent->bringToFront();
	}

	else
	{
		if(parentUiManager == NULL)
			std::cout << "UiObject::bringToFront() called without a valid parentUiManager (is NULL)." << std::endl;

		if(parentUiManager->childList.front() != shared_from_this())
		{
			parentUiManager->childList.remove(shared_from_this());
			parentUiManager->childList.push_front(shared_from_this());
		}
	}
}

void UiObject::setUiObjectTexture(std::shared_ptr<SdlTexture> texture)
{
	uiTexture = texture;
}

void UiObject::setUiObjectOffset(int x, int y)
{
	absoluteX -= xOffset;
	absoluteY -= yOffset;

	xOffset = x;
	yOffset = y;

	absoluteX += xOffset;
	absoluteY += yOffset;
}

void UiObject::setUiObjectSize(int width, int height)
{
	this->width = width;
	this->height = height;
	logicalWidth = this->width;
	logicalHeight = this->height;
}

void UiObject::setUiObjectLogicalSize(int logicalWidth, int logicalHeight)
{
	this->logicalWidth = logicalWidth;
	this->logicalHeight = logicalHeight;
}

void UiObject::setParentUiManager(UiManager *uiMngr)
{
	parentUiManager = uiMngr;

	for(std::shared_ptr<UiObject> childUiObj : childList)
		childUiObj->setParentUiManager(uiMngr);
}

void UiObject::getUiObjectOffset(int &xOff, int &yOff) const
{
	xOff = xOffset;
	yOff = yOffset;
}

std::shared_ptr<SdlTexture> UiObject::getTexture()
{
	return uiTexture;
}

int UiObject::getWidth() const
{
	return width;
}

int UiObject::getHeight() const
{
	return height;
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
			std::cout << "UiObject::render() called without a valid parentUiManager (is NULL)." << std::endl;

		uiTexture->renderFitToArea(parentUiManager->getRenderer(), absoluteX, absoluteY, width, height);
	}

	if(postRenderProcedure)
		postRenderProcedure();

	for(auto it = childList.rbegin(); it != childList.rend(); ++it)
	{
		(*it)->render(absoluteX, absoluteY);
	}
}

void UiObject::renderScaled(int parentX, int parentY, double widthScale, double heightScale)
{
	absoluteX = parentX + xOffset;
	absoluteY = parentY + yOffset;

	scaleW = widthScale;
	scaleH = heightScale;

	if(preRenderProcedure)
		preRenderProcedure();

	if(uiTexture != NULL)
	{
		if(parentUiManager == NULL)
			std::cout << "UiObject::renderScaled() called without a valid parentUiManager (is NULL)." << std::endl;

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

void UiObject::setSdlEventHandler(std::function<bool(const SDL_Event &e)> eventHandler)
{
	evtHandler = eventHandler;
}

bool UiObject::handleSdlEventMouse(const SDL_Event &e)
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

		// if mouse wasn't inside any of my child's areas, it's on me
		mouseOnTop = this;

		// therefore handle with my handler. always return mouse event as handled
		if(evtHandler)
			evtHandler(e);

		if(e.type == SDL_MOUSEBUTTONUP)
		{
			if(parentUiManager == NULL)
				std::cout << "UiObject::handleSdlEventMouse() called without a valid parentUiManager (is NULL)." << std::endl;
			
			parentUiManager->setFocusedUiObject(this);

			bringToFront();
		}

		return true;
	}

	return false;
}

bool UiObject::handleSdlEventKeyboard(const SDL_Event &e)
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

bool UiObject::isMouseInside() const
{
	int x, y;

	SDL_GetMouseState(&x, &y);

	if(parentUiManager == NULL)
			std::cout << "UiObject::isMouseInside() called without a valid parentUiManager (is NULL)." << std::endl;

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

bool UiObject::getRelativeMousePos(const UiObject &obj, int &x, int &y)
{
	SDL_GetMouseState(&x, &y);

	if(obj.parentUiManager == NULL)
			std::cout << "UiObject::getRelativeMousePos() called without a valid parentUiManager (is NULL) for the provided argument UiObject *obj." << std::endl;

	x = (x - obj.absoluteX * obj.parentUiManager->getWindowScaleW()) / obj.parentUiManager->getWindowScaleW();
	y = (y - obj.absoluteY * obj.parentUiManager->getWindowScaleH()) / obj.parentUiManager->getWindowScaleH();

	if(	x < 0 || x > obj.width || 
		y < 0 || y > obj.height)
	{
		std::cout << "UiObject::getRelativeMousePos() returned invalid x and/or y: x = " << x << " y = " << y << " (obj: width = " << obj.width << " height = " << obj.height << ")." << std::endl;

		x = -1;
		y = -1;

		return false;
	}

	else
		return true;
}
