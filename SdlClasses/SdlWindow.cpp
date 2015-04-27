#include "SdlWindow.h"

#include <stdio.h>

SdlWindow::SdlWindow(char* title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags)
{
	window = SDL_CreateWindow(	title,
								x,
								y,
								w,
								h,
								windowFlags);
	if(window == NULL)
		printf("%s could not be created! SDL Error: %s\n", title, SDL_GetError());
	
	else
	{
		//Create renderer for window
		renderer = SDL_CreateRenderer(window, -1, rendererFlags);
		if(renderer == NULL)
		{
			printf("renderer for %s could not be created! SDL Error: %s\n", title, SDL_GetError());
			
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

	//textureMap["walkTexture"]->clearTexture();		// crash if SDLClose calls this function after textureMap.clear() at main
													// (but textureMap.clear() must happen before SDL systems are terminated)
}

void SdlWindow::handleEvent(SDL_Event& e)
{
	// If an event was detected for this window
	if(e.type == SDL_WINDOWEVENT && e.window.windowID == windowID)
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

			// Window minimized
			case SDL_WINDOWEVENT_MINIMIZED:
				minimized = true;
			break;

			// Window maxized
			case SDL_WINDOWEVENT_MAXIMIZED:
				minimized = false;
			break;
			
			// Window restored
			case SDL_WINDOWEVENT_RESTORED:
				minimized = false;
			break;

			// Hide on close
			case SDL_WINDOWEVENT_CLOSE:
				SDL_HideWindow(window);
			break;
		}
	}

	// handle other events with the implemented handler
	handleImplementedEvents(e);
}

void SdlWindow::focus()
{
    //Restore window if needed
    if(!shown)
    {
        SDL_ShowWindow(window);
    }

    //Move window forward
    SDL_RaiseWindow(window);
}

void SdlWindow::render()
{
    if(!minimized)
    {    
        //Clear screen
        //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        //SDL_RenderClear(renderer);

        //Update screen
        SDL_RenderPresent(renderer);
    }
}

int SdlWindow::getWidth()
{
	return width;
}

int SdlWindow::getHeight()
{
	return height;
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


/*void handleWalkWindowEvent(SDL_Event windowEvent)
{
	bool updateScreen = false;

	switch(windowEvent.type)
	{
		case SDL_WINDOWEVENT:
			if(windowEvent.window.event == SDL_WINDOWEVENT_CLOSE)
				destroyWalkWindow();
		break;

		case SDL_KEYDOWN:
			switch(windowEvent.key.keysym.sym)
			{
				case SDLK_UP:
					if(walkY - 1 < 0)
						walkY = 0;

					else if(map.Tile(walkX, walkY - 1).getH() > map.getSeaLvl())
						walkY--;
					updateScreen = true;
				break;

				case SDLK_DOWN:
					if(walkY + 1 >= map.getMapHeight())
						walkY = map.getMapHeight() - 1;

					else if(map.Tile(walkX, walkY + 1).getH() > map.getSeaLvl())
						walkY++;
					updateScreen = true;
				break;

				case SDLK_LEFT:
					if(map.Tile(walkX - 1, walkY).getH() > map.getSeaLvl())
					{
						if(walkX - 1 < 0)
							walkX = map.getMapWidth() - 1;
						else
							walkX--;
					}

					updateScreen = true;
				break;

				case SDLK_RIGHT:
					if(map.Tile(walkX + 1, walkY).getH() > map.getSeaLvl())
					{
						if(walkX + 1 >= map.getMapWidth())
							walkX = 0;
						
						else
							walkX++;
					}

					updateScreen = true;
				break;
			}
		break;
	}

	if(updateScreen)
	{
		updateWalkTex();
		textureMap["walkTexture"]->render(renderer, 0, 0);
		SDL_RenderPresent(renderer);
	}
}*/
