#include <iostream>
#include <iomanip>

#include <unordered_map>

#include <stdlib.h>
#include <time.h>

#include "Map.h"

#include "SdlClasses/SdlWindows/NoiseWindow.h"
//#include "SdlClasses/SdlWindows/WalkWindow.h"

#include <SDL2/SDL.h>

using namespace std;

SDL_Event event;

NoiseWindow *noiseWindow;

const int mapNum = 3;

Map worldMap(1025, 1025);
Map worldMap2(1025, 1025);
Map worldMap3(1025, 1025);
Map* worldMapsVect[mapNum] = {&worldMap, &worldMap2, &worldMap3};

//funções SDL
bool SDLStart();
void SDLClose();

void doFrame(bool draw);
void resyncFrameTime();
void regulateFrameRate();

int main(int argc, char* args[])
{
	/*// seed manual (barra no começo dessa linha para mudar)
	int seed;
	cout << "Seed: ";
	cin >> seed;
	cout << endl;

	srand(seed);
	/*/srand(time(NULL));//*/

	if(!SDLStart())
	{
		cout << "ERRO AO INICIALIZAR SDL" << endl;

		return -1;
	}

	noiseWindow = new NoiseWindow(worldMapsVect, mapNum);

	// while window is open
	while(noiseWindow->isShown())
	{
		while(SDL_PollEvent(&event))
		{
			noiseWindow->handleSdlEvent(event);
		}

		regulateFrameRate();
	}

	delete noiseWindow;

	SDLClose();

	return 0;
}

bool SDLStart()
{
	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;

		return false;
	}

	//Initialize SDL_ttf
	if(TTF_Init() == -1)
	{
		cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
		return false;
	}

	return true;
}

void SDLClose()
{
	TTF_Quit();
	//Quit SDL subsystems
	SDL_Quit();
}


// ----- Frame regulator: http://www.cplusplus.com/forum/beginner/94946/ -- by Disch ----- //

/*
	you need a 'doframe' function with an optional 'draw' parameter.
	This is where you will put all your game code.
*/
void doFrame(bool draw)
{
	if(draw)
		noiseWindow->doRefresh();
}

/*
then to regulate the framerate, you need some variables.  I'm making these global for
simplicity of this example, but a better approach would be to put all of this in a class.
*/

// timestamp that next frame is to occur
Uint32 nextFrame;

// the number of frames we have skipped
int framesSkipped;

// configurable (optionally const) value to determine how many frames we allow it to
//   "fall behind" before we give up and resync.
// higher values = game opts to run 'slower'
// lower values = game opts to run 'choppier'
int maxFrameSkip = 5;

// configurable (optionally const) value to determine when to start "skipping" frames.
//   IE, we'll start skipping after we fall this many frames behind
int skipFramesBehind = 5;

// number of milliseconds between frames.  17 gives you approx 60 FPS  (1000 / 60)
int frameLength = 5;


/*
We also need a function to "resync".  This should be called once to initialize the timing
mechanism.  And also called again whenever something happens that throws the
clock way out of whack (such as the user entering a menu or something).  This probably
does not apply to SDL much so you probably will only need to call this on initialization.
*/

void resyncFrameTime()
{
	nextFrame = SDL_GetTicks();
	framesSkipped = 0;
}


/*
Now the actual function to regulate the framerate
I'm going to use
*/

void regulateFrameRate()
{
	// get time between next frame and now
	Sint32 dif = (Sint32)(SDL_GetTicks() - nextFrame);

	// if it's not yet time for the next frame... wait and sleep for a bit
	if(dif < 0)
	{
		SDL_Delay(1);
		return;
	}

	// otherwise, we're going to do at least 1 frame.  increment our time counter
	nextFrame += frameLength;

	// if we have fallen too far behind.. we need to start skipping frames to catch up
	if(dif >= (frameLength * skipFramesBehind))
	{
		// if we have already skipped the maximum number of times.  "Give up" and just resync
		if(framesSkipped >= maxFrameSkip)
			resyncFrameTime();
		else
		{
			// otherwise, skip this frame
			++framesSkipped;
			doFrame(false);    // by "skip", I mean "run logic, but don't draw".
			return;
		}
	}

	// otherwise, we just do a "normal" frame
	framesSkipped = 0;
	doFrame(true);  // do the frame, and draw it
}
