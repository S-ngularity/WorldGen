#include "DefaultUiObjects/UiButton.h"

#include "SdlClasses/SdlTexture.h"

#include "DefaultUiObjects/UiLabel.h"

UiButton::UiButton(int xOff, int yOff, UiLabel *textLabel, std::shared_ptr<SdlTexture> t, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	label = textLabel;

	if(getTexture() != NULL)
	{
		standardClipRect = std::make_shared<SDL_Rect>();
		*standardClipRect = {0, 0, t->getWidth()/3, t->getHeight() };
		
		hoverClipRect = std::make_shared<SDL_Rect>();
		*hoverClipRect = {t->getWidth()/3, 0, t->getWidth()/3, t->getHeight()};
		
		pressedClipRect = std::make_shared<SDL_Rect>();
		*pressedClipRect = {t->getWidth()/3 * 2, 0, t->getWidth()/3, t->getHeight()};
	}

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });

	if(label != NULL)
	{
		label->setAlignMode(ALIGN_CENTER_CENTER);
		addChild(label);
	}
}

UiButton::UiButton(int xOff, int yOff, int w, int h, UiLabel *textLabel, std::shared_ptr<SdlTexture> t, std::function<void()> btEvtH) : 
	UiObject(xOff, yOff, w, h, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	label = textLabel;
	
	if(getTexture() != NULL)
	{
		standardClipRect = std::make_shared<SDL_Rect>();
		*standardClipRect = {0, 0, t->getWidth()/3, t->getHeight() };
		
		hoverClipRect = std::make_shared<SDL_Rect>();
		*hoverClipRect = {t->getWidth()/3, 0, t->getWidth()/3, t->getHeight()};
		
		pressedClipRect = std::make_shared<SDL_Rect>();
		*pressedClipRect = {t->getWidth()/3 * 2, 0, t->getWidth()/3, t->getHeight()};
	}

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });

	if(label != NULL)
	{
		label->setAlignMode(ALIGN_CENTER_CENTER);
		addChild(label);
	}
}

bool UiButton::buttonEvtHandler(SDL_Event &e)
{
	switch(e.type)
	{
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
				case SDLK_RETURN:
				case SDLK_KP_ENTER:
					bool mouseIsPressed = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);

					if(customButtonAction && !mouseIsPressed)
						customButtonAction();
				break;
			}
		break;

		case SDL_MOUSEMOTION:
			return true;
		break;

		case SDL_MOUSEBUTTONDOWN:
			clickHappenedHere = true;

			return true;
		break;

		case SDL_MOUSEBUTTONUP:
			if(clickHappenedHere && customButtonAction)
				customButtonAction();

			clickHappenedHere = false;

			return true;
		break;

		default:
			return false;
	}

	return false;
}

void UiButton::buttonPreRender()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	bool enterIsPressed = state[SDL_SCANCODE_KP_ENTER] || state[SDL_SCANCODE_RETURN];
	bool mouseIsPressed = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool buttonIsFocused = parentUiManager->getFocusedUiObject() == this;

	if(!mouseIsPressed)
		clickHappenedHere = false;


	if((clickHappenedHere && mouseIsPressed && UiObject::mouseOnTop == this) || 
		(buttonIsFocused && enterIsPressed && !mouseIsPressed))
	{
		isPressed = true;
	}

	else
		isPressed = false;

	if(getTexture() != NULL)
	{
		if(isPressed)
			getTexture()->setCropRect(pressedClipRect);

		else if(UiObject::mouseOnTop == this && !isPressed)
			getTexture()->setCropRect(hoverClipRect);

		else
			getTexture()->setCropRect(standardClipRect);
	}

	if(label != NULL)
	{
		int myX, myY;
		getUiObjectOffset(&myX, &myY);
		
		label->setUiObjectOffset(getWidth() / 2.f, getHeight() / 2.f);
	}
}
