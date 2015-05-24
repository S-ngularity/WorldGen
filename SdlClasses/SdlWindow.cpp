#include "SdlClasses/SdlWindow.h"

#include <iostream>

SdlWindow::SdlWindow(char const *title, int x, int y, int w, int h, int resW, int resH, Uint32 windowFlags, Uint32 rendererFlags)
{
	evtHandler = nullptr;

	originalWndWidth = w;
	originalWndHeight = h;
	windowWidth = w;
	windowHeight = h;
	resolutionWidth = resW;
	resolutionHeight = resH;

	mouseFocus = false;
	keyboardFocus = false;
	minimized = false;
	shown = false;

	refreshSignaled = false;

	window = SDL_CreateWindow(	title,
								x,
								y,
								w,
								h,
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

			SDL_Texture *resTex = SDL_CreateTexture(wndRenderer, SDL_PIXELFORMAT_RGBA8888, 
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
				resolutionTexture.setTexture(resTex, resolutionWidth, resolutionHeight);

				resolutionTexture.setAsRenderTarget(wndRenderer);
				SDL_SetRenderDrawColor(wndRenderer, 255, 0, 255, 255);
				SDL_RenderClear(wndRenderer);
				resolutionTexture.releaseRenderTarget(wndRenderer);

				gui = new UiObject(wndRenderer, 0, 0, resW, resH); // create base gui object
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

void SdlWindow::addChildWindow(SdlWindow *c)
{
	childList.push_front(c);
}

void SdlWindow::setWindowSdlEvtHandler(std::function<bool(SDL_Event& e)> evth)
{
	evtHandler = evth;
}

bool SdlWindow::handleSdlEvent(SDL_Event& e)
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
				case SDL_WINDOWEVENT_RESIZED:
					windowWidth = e.window.data1;
					windowHeight = e.window.data2;
					gui->setWindowScale(getWindowWidthScale(), getWindowHeightScale());
					signalRefresh();
				break;

				// Repaint on expose
				case SDL_WINDOWEVENT_EXPOSED:
					signalRefresh();
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
				break;

				default:
				break;
			}
		}

		else
		{
			// handle other events with the implemented handler
			gui->handleSdlEvent(e);

			// handle other events with the implemented handler
			if(evtHandler)
				evtHandler(e);
		}

		return true;
	} // event had this window's windowId

	return false;
}

SDL_Renderer* SdlWindow::getRenderer()
{
	return wndRenderer;
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

void SdlWindow::signalRefresh()
{
	refreshSignaled = true;
}

void SdlWindow::doRefresh()
{
	if(refreshSignaled)
	{
		if(!minimized && shown)
		{
			resolutionTexture.setAsRenderTarget(wndRenderer);
			SDL_SetRenderDrawColor(wndRenderer, 255, 0, 255, 255);
			SDL_RenderClear(wndRenderer);
			gui->render(0, 0);
			resolutionTexture.releaseRenderTarget(wndRenderer);

			resolutionTexture.renderFitToArea(wndRenderer, 0, 0, windowWidth, windowHeight);

			SDL_RenderPresent(wndRenderer);
		}

		for(SdlWindow *childWindow : childList)
			childWindow->doRefresh();

		refreshSignaled = false;
	}
}

int SdlWindow::getWindowWidth()
{
	return windowWidth;
}

int SdlWindow::getWindowHeight()
{
	return windowHeight;
}

double SdlWindow::getWindowWidthScale()
{
	return (double)windowWidth / (double)originalWndWidth;
}

double SdlWindow::getWindowHeightScale()
{
	return (double)windowHeight / (double)originalWndHeight;
}

bool SdlWindow::hasMouseFocus()
{
	return mouseFocus;
}

bool SdlWindow::hasKeyboardFocus()
{
	return keyboardFocus;
}

bool SdlWindow::isMinimized()
{
	return minimized;
}

bool SdlWindow::isShown()
{
	return shown;
}
