#include "SdlClasses/SdlWindow.h"

#include <iostream>

SdlWindow::SdlWindow(char const *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags)
{
	evtHandler = NULL;
	
	width = w;
	height = h;
	originalWidth = w;
	originalHeight = h;

	mouseFocus = false;
	keyboardFocus = false;
	minimized = false;
	shown = false;

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
		renderer = SDL_CreateRenderer(window, -1, rendererFlags);
		
		if(renderer == NULL)
		{
			std::cout << "Renderer for " << title << " could not be created! SDL Error: " << SDL_GetError() << std::endl;
			
			SDL_DestroyWindow(window);
			window = NULL;
		}

		else
		{
			windowID = SDL_GetWindowID(window);

			shown = true;
		}
	}
}

SdlWindow::~SdlWindow()
{
	if(window != NULL)
		SDL_DestroyWindow(window);

	if(renderer != NULL)
		SDL_DestroyRenderer(renderer);

	renderer = NULL;
	window = NULL;
}

void SdlWindow::setWindowSdlEvtHandler(std::function<void(SDL_Event& e)> evth)
{
	evtHandler = evth;
}

void SdlWindow::handleEvent(SDL_Event& e)
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
					width = e.window.data1;
					height = e.window.data2;
					SDL_RenderPresent(renderer);
				break;

				// Repaint on expose
				case SDL_WINDOWEVENT_EXPOSED:
					SDL_RenderPresent(renderer);
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
			}
		}
	}

	// handle other events with the implemented handler
	if(evtHandler)
		evtHandler(e);
}

SDL_Renderer* SdlWindow::getRenderer()
{
	return renderer;
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

void SdlWindow::refresh()
{
	if(!minimized && shown)
		SDL_RenderPresent(renderer);
}

int SdlWindow::getWindowWidth()
{
	return width;
}

int SdlWindow::getWindowHeight()
{
	return height;
}

double SdlWindow::getWindowWidthScale()
{
	return width / (double)originalWidth;
}

double SdlWindow::getWindowHeightScale()
{
	return height / (double)originalHeight;
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
