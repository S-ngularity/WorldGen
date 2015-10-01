#include "DefaultUiObjects/UiPanel.h"

#include "SdlClasses/SdlTexture.h"

UiPanel::UiPanel(int xOff, int yOff, SdlTexture *t) :  
	UiObject(xOff, yOff, t, nullptr)
{}

UiPanel::UiPanel(int xOff, int yOff, int w, int h, SdlTexture *t) : 
	UiObject(xOff, yOff, w, h, t, nullptr)
{}
