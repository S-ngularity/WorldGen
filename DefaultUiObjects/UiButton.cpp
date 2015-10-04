#include "DefaultUiObjects/UiButton.h"

#include "SdlClasses/SdlTexture.h"

#include "DefaultUiObjects/UiLabel.h"

UiButton::UiButton(int xOff, int yOff, UiLabel *textLabel, std::shared_ptr<SdlTexture> t, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	label = textLabel;
	btTexture = t;
	btTextureHover = NULL;
	btTexturePressed = NULL;

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
	btTexture = t;
	btTextureHover = NULL;
	btTexturePressed = NULL;

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });

	if(label != NULL)
	{
		label->setAlignMode(ALIGN_CENTER_CENTER);
		addChild(label);
	}
}

UiButton::UiButton(int xOff, int yOff, UiLabel *textLabel, std::shared_ptr<SdlTexture> t, std::shared_ptr<SdlTexture> tHover, std::shared_ptr<SdlTexture> tPressed, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	label = textLabel;
	btTexture = t;
	btTextureHover = tHover;
	btTexturePressed = tPressed;

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });

	if(label != NULL)
	{
		label->setAlignMode(ALIGN_CENTER_CENTER);
		addChild(label);
	}
}

UiButton::UiButton(int xOff, int yOff, int w, int h, UiLabel *textLabel, std::shared_ptr<SdlTexture> t, std::shared_ptr<SdlTexture> tHover, std::shared_ptr<SdlTexture> tPressed, std::function<void()> btEvtH) : 
	UiObject(xOff, yOff, w, h, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	label = textLabel;
	btTexture = t;
	btTextureHover = tHover;
	btTexturePressed = tPressed;

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


	if(isPressed)
	{
		if(btTexturePressed != NULL)
			setUiObjectTexture(btTexturePressed);
	}

	else if(UiObject::mouseOnTop == this && !mouseIsPressed)
	{
		if(btTextureHover != NULL)
			setUiObjectTexture(btTextureHover);
	}

	else
	{
		if(btTexture != NULL)
			setUiObjectTexture(btTexture);
	}

	if(label != NULL)
	{
		int myX, myY;
		getUiObjectOffset(&myX, &myY);
		
		label->setUiObjectOffset(getWidth() / 2.f, getHeight() / 2.f);
	}
}
