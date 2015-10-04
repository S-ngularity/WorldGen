#include "DefaultUiObjects/UiPanel.h"

#include "SdlClasses/SdlTexture.h"

UiPanel::UiPanel(int xOff, int yOff, std::shared_ptr<SdlTexture> t) :  
	UiObject(xOff, yOff, t, nullptr)
{}

UiPanel::UiPanel(int xOff, int yOff, int w, int h, std::shared_ptr<SdlTexture> t) : 
	UiObject(xOff, yOff, w, h, t, nullptr)
{}
