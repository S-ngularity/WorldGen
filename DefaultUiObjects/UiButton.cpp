#include "DefaultUiObjects/UiButton.h"

#include "SdlClasses/SdlTexture.h"

#include "DefaultUiObjects/UiLabel.h"

UiButton::UiButton(int xOff, int yOff, int w, int h, std::shared_ptr<UiLabel> textLabel, std::shared_ptr<SdlTexture> t, std::function<void()> btAction) : 
	UiObject(xOff, yOff, w, h, t, [&](const SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btAction)
{
	labelPtr = textLabel.get();
	
	if(getTexture() != NULL)
	{
		standardClipRect = std::make_shared<SDL_Rect>();
		*standardClipRect = {0, 0, t->getWidth(), t->getHeight()/3 };
		
		hoverClipRect = std::make_shared<SDL_Rect>();
		*hoverClipRect = {0, t->getHeight()/3, t->getWidth(), t->getHeight()/3};
		
		pressedClipRect = std::make_shared<SDL_Rect>();
		*pressedClipRect = {0, t->getHeight()/3 * 2, t->getWidth(), t->getHeight()/3};
	}

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });

	if(textLabel != NULL)
	{
		textLabel->setAlignMode(ALIGN_CENTER_CENTER);
		addChild(textLabel);
	}
}

bool UiButton::buttonEvtHandler(const SDL_Event &e)
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

	if(labelPtr != NULL)
	{
		int myX, myY;
		getUiObjectOffset(myX, myY);
		
		labelPtr->setUiObjectOffset(getWidth() / 2.f, getHeight() / 2.f);
	}
}
