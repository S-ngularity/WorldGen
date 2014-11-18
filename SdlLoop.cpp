#include <iostream>
#include <SdlLoop.h>

SdlLoop::SdlLoop()
{
	state = readTectonics;
}

void SdlLoop::readTectonics()
{
	do{
		cout << endl <<  "Tectonics iterations (~30-60): ";
		cin >> numIts;
		cout << endl;
	}while(numIts <= 0);

	state = doTectonics;
}

void SdlLoop::doTectonics()
{
	tectonics(iteration, numIts);
	iteration++;

	if((int)(100 * ((float)iteration / (float)numIts)) > percentComplete)
	{
		percentComplete = (int)(100 * ((float)iteration / (float)numIts));

		cout << "\b\b\b" << percentComplete << "%";

		if(percentComplete % RENDERWHENMOD == 0)
			renderMapNoSea();
	}

	if(iteration == numIts)
	{
		highestH = 0;

		for(int y = 0; y < MAPHEIGHT; y++)
			for(int x = 0; x < MAPWIDTH; x++)
				if(map.Tile(x, y).getH() > highestH)
					highestH = map.Tile(x, y).getH();

		cout << endl << endl << "Highest point: " << highestH << endl << endl;

		renderMapNoSea();
		SDL_RenderPresent(Renderer);

		state = readErosion;
	}
}

void SdlLoop::readErosion()
{
	do{
		cout << endl << "Erosion iterations (~500-5000): ";
		cin >> numIts;
		cout << endl;
	}while(numIts < 0);

	iteration = 0;
	percentComplete = 0;

	if(numIts == 0)
		state = done;

	else
		state = doErosion;
}

void SdlLoop::doErosion()
{
	erosion(iteration, numIts);
	iteration++;

	if((int)(100 * ((float)iteration / (float)numIts)) > percentComplete)
	{
		percentComplete = (int)(100 * ((float)iteration / (float)numIts));

		cout << "\b\b\b" << percentComplete << "%";

		if(percentComplete % printWhenMod == 0)
			renderMapNoSea();
	}

	if(iteration == numIts)
	{
		doErosion = false;
		readTectonics = false;

		highestH = 0;

		for(int y = 0; y < MAPHEIGHT; y++)
			for(int x = 0; x < MAPWIDTH; x++)
				if(map.Tile(x, y).getH() > highestH)
					highestH = map.Tile(x, y).getH();

		cout << endl << endl << "Highest point: " << highestH << endl << endl;

		//renderMapNoSea();
		renderMap();
		SDL_RenderPresent(Renderer);

		state = done;
	}
}

SdlLoop::loop()
{
	bool quit = false;
	int numIts, iteration;

	while(!quit)
	{
		switch(state)
		{
			case readTectonics:
				readTectonics();
			break;

			case doTectonics:
				doTectonics();
			break;

			case readErosion:
				readErosion();
			break;

			case doErosion:
				doErosion();
			break;
		}
	}
}