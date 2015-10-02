#include "DefaultUiObjects/UiButton.h"

#include "SdlClasses/SdlTexture.h"

UiButton::UiButton(int xOff, int yOff, SdlTexture *t, std::function<void()> btEvtH) :  
	UiObject(xOff, yOff, t, [&](SDL_Event &e){ return buttonEvtHandler(e); }),
	customButtonAction(btEvtH)
{
	btTexture = t;
	btTextureHover = NULL;
	btTexturePressed = NULL;

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
					if(customButtonAction)
						customButtonAction();
				break;
			}
		break;

		case SDL_MOUSEMOTION:
			return true;
		break;

		case SDL_MOUSEBUTTONDOWN:
			isPressed = true;

			return true;
		break;

		case SDL_MOUSEBUTTONUP:
			if(isPressed && customButtonAction)
				customButtonAction();
			
			isPressed = false;

			return true;
		break;

		default:
			return false;
	}

	return false;
}

void UiButton::buttonPreRender()
{
	if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		isPressed = false;

	if(UiObject::mouseOnTop == this)
	{
		if(isPressed)
		{
			if(btTexturePressed != NULL)
				setUiObjectTextureNoDelete(btTexturePressed);
		}

		else
		{
			if(btTextureHover != NULL)
				setUiObjectTextureNoDelete(btTextureHover);
		}
	}

	else
		setUiObjectTextureNoDelete(btTexture);
}
