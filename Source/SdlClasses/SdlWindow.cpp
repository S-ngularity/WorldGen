#include "SdlClasses/SdlWindow.h"

#include <iostream>

SdlWindow::SdlWindow(char const *title, int x, int y, int width, int height, int resolutionWidth, int resolutionHeight, Uint32 windowFlags, Uint32 rendererFlags)
{
	evtHandler = nullptr;

	originalWndWidth = width;
	originalWndHeight = height;
	windowWidth = width;
	windowHeight = height;
	this->resolutionWidth = resolutionWidth;
	this->resolutionHeight = resolutionHeight;

	mouseFocus = false;
	keyboardFocus = false;
	minimized = false;
	shown = false;

	window = SDL_CreateWindow(	title,
								x,
								y,
								width,
								height,
								windowFlags);
	
	if(window == NULL)
		std::cout << title << " could not be created! SDL Error: " << SDL_GetError() << std::endl;
	
	else
	{
		//Create renderer for window
		wndRenderer = SDL_CreateRenderer(window, -1, rendererFlags | SDL_RENDERER_TARGETTEXTURE);
		
		if(wndRenderer == NULL)
		{
			std::cout << "Renderer for " << title << " could not be created! SDL Error: " << SDL_GetError() << std::endl;
			
			SDL_DestroyWindow(window);
			window = NULL;
		}

		else
		{
			windowID = SDL_GetWindowID(window);
			shown = true;

			SDL_Texture *resTex = SDL_CreateTexture(wndRenderer, SDL_PIXELFORMAT_ARGB8888, 
													SDL_TEXTUREACCESS_TARGET, 
													resolutionWidth, resolutionHeight);
			if(resTex == NULL)
			{
				std::cout << "Resolution texture for " << title << " could not be created! SDL Error: " << SDL_GetError() << std::endl;
				
				SDL_DestroyRenderer(wndRenderer);
				SDL_DestroyWindow(window);
				wndRenderer = NULL;
				window = NULL;
			}

			else
			{
				resolutionTexture.setTexture(resTex);

				resolutionTexture.setAsRenderTarget(wndRenderer);
				SDL_SetRenderDrawColor(wndRenderer, 255, 0, 255, 255);
				SDL_RenderClear(wndRenderer);
				resolutionTexture.releaseRenderTarget(wndRenderer);

				windowUiManager = std::make_unique<UiManager>(wndRenderer, resolutionWidth, resolutionHeight, getWindowWidthScale(), getWindowHeightScale()); // create base windowUiManager object
			}

		}
	}
}

SdlWindow::~SdlWindow()
{
	if(window != NULL)
		SDL_DestroyWindow(window);

	if(wndRenderer != NULL)
		SDL_DestroyRenderer(wndRenderer);

	wndRenderer = NULL;
	window = NULL;
}

void SdlWindow::setWindowSdlEvtHandler(std::function<bool(const SDL_Event &e)> eventHandler)
{
	evtHandler = eventHandler;
}

bool SdlWindow::handleSdlEvent(const SDL_Event &e)
{
	// If an event was detected for this window
	if(e.window.windowID == windowID)
	{
		if(e.type == SDL_WINDOWEVENT)
		{
			switch(e.window.event)
			{
				// Window appeared
				case SDL_WINDOWEVENT_SHOWN:
					shown = true;
				break;

				// Window disappeared
				case SDL_WINDOWEVENT_HIDDEN:
					shown = false;
				break;

				// Get new dimensions and repaint
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					windowWidth = e.window.data1;
					windowHeight = e.window.data2;
					windowUiManager->setWindowScale(getWindowWidthScale(), getWindowHeightScale());
				break;

				// Repaint on expose
				case SDL_WINDOWEVENT_EXPOSED:
					doRefresh();
				break;

				// Mouse enter
				case SDL_WINDOWEVENT_ENTER:
					mouseFocus = true;
				break;
				
				// Mouse exit
				case SDL_WINDOWEVENT_LEAVE:
					mouseFocus = false;
				break;

				// Keyboard focus gained
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					keyboardFocus = true;
				break;
				
				// Keyboard focus lost
				case SDL_WINDOWEVENT_FOCUS_LOST:
					keyboardFocus = false;
				break;

				case SDL_WINDOWEVENT_MINIMIZED:
					minimized = true;
				break;

				case SDL_WINDOWEVENT_MAXIMIZED:
					minimized = false;
				break;
				
				case SDL_WINDOWEVENT_RESTORED:
					minimized = false;
				break;

				// Hide on close
				case SDL_WINDOWEVENT_CLOSE:
					SDL_HideWindow(window);
					shown = false;
				break;

				default:
				break;
			}
		}

		else
		{
			// handle other events with the implemented Ui handlers
			bool uiHandled = windowUiManager->handleSdlEvent(e);

			// if Ui didn't handle the event, use the implemented window handler
			if(!uiHandled && evtHandler)
				evtHandler(e);
		}

		return true;
	} // event had this window's windowId

	return false;
}

void SdlWindow::doRefresh()
{
	if(!minimized && shown)
	{
		resolutionTexture.setAsRenderTarget(wndRenderer);
		SDL_SetRenderDrawColor(wndRenderer, 255, 0, 255, 255);
		SDL_RenderClear(wndRenderer);
		windowUiManager->render();
		resolutionTexture.releaseRenderTarget(wndRenderer);

		resolutionTexture.renderFitToArea(wndRenderer, 0, 0, windowWidth, windowHeight);

		SDL_RenderPresent(wndRenderer);
	}
}

void SdlWindow::show()
{
	if(!shown)
	{
		shown = true;
		SDL_ShowWindow(window);
	}

	// always move window forward
	SDL_RaiseWindow(window);
}

void SdlWindow::hide()
{
	shown = false;

	SDL_HideWindow(window);
}

SDL_Renderer* SdlWindow::getRenderer()
{
	return wndRenderer;
}

int SdlWindow::getWindowWidth() const
{
	return windowWidth;
}

int SdlWindow::getWindowHeight() const
{
	return windowHeight;
}

double SdlWindow::getWindowWidthScale() const
{
	return (double)windowWidth / (double)originalWndWidth;
}

double SdlWindow::getWindowHeightScale() const
{
	return (double)windowHeight / (double)originalWndHeight;
}

bool SdlWindow::hasMouseFocus() const
{
	return mouseFocus;
}

bool SdlWindow::hasKeyboardFocus() const
{
	return keyboardFocus;
}

bool SdlWindow::isMinimized() const
{
	return minimized;
}

bool SdlWindow::isShown() const
{
	return shown;
}
