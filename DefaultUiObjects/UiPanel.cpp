#include "DefaultUiObjects/UiPanel.h"

#include "SdlClasses/SdlTexture.h"

UiPanel::UiPanel(int xOff, int yOff, int width, int height, std::shared_ptr<SdlTexture> texture) : 
	UiObject(xOff, yOff, width, height, texture, nullptr)
{}
