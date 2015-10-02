#include "DefaultUiObjects/UiButton.h"

#include "SdlClasses/SdlTexture.h"

UiButton::UiButton(int xOff, int yOff, SdlTexture *t, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	btTexture = t;
	btTextureHover = NULL;
	btTexturePressed = NULL;

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });
}

UiButton::UiButton(int xOff, int yOff, int w, int h, SdlTexture *t, std::function<void()> btEvtH) : 
	UiObject(xOff, yOff, w, h, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	btTexture = t;
	btTextureHover = NULL;
	btTexturePressed = NULL;

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });
}

UiButton::UiButton(int xOff, int yOff, SdlTexture *t, SdlTexture *tHover, SdlTexture *tPressed, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	btTexture = t;
	btTextureHover = tHover;
	btTexturePressed = tPressed;

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });
}

UiButton::UiButton(int xOff, int yOff, int w, int h, SdlTexture *t, SdlTexture *tHover, SdlTexture *tPressed, std::function<void()> btEvtH) : 
	UiObject(xOff, yOff, w, h, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	btTexture = t;
	btTextureHover = tHover;
	btTexturePressed = tPressed;

	clickHappenedHere = false;
	isPressed = false;

	setPreRenderProcedure([&](){ buttonPreRender(); });
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
			setUiObjectTextureNoDelete(btTexturePressed);
	}

	else if(UiObject::mouseOnTop == this && !mouseIsPressed)
	{
		if(btTextureHover != NULL)
			setUiObjectTextureNoDelete(btTextureHover);
	}

	else
	{
		if(btTexture != NULL)
			setUiObjectTextureNoDelete(btTexture);
	}
}
