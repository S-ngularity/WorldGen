#include "OpenSimplexNoise.h"
#include "Map/Map.h"

#include "Ui/UiEvents/NoiseInfoRequest.h"

#include <iostream>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

OpenSimplexNoise::OpenSimplexNoise(Map *theMap, int oct, double freq, double pers, double fdiv) : 
	Noise("OpenSimplex")
{
	EvtAggr::subscribe<UiCode>( [&](UiCode &c){ handleEvtCode(c); } );
	EvtAggr::subscribe<NoiseInfoRequest*>( [&](NoiseInfoRequest* &n){ n->setInfo(octaves, frequency, persistence, freqDiv); } );
	//srand(time(NULL));

	map = theMap;
	alreadySaved = false;
	
	reset();

	octaves = oct;
	frequency = freq;
	persistence = pers;
	freqDiv = fdiv;

	EvtAggr::publish<UiCode>(UiCode(UIEVT_NOISEINFOUPDATED));
}

OpenSimplexNoise::~OpenSimplexNoise()
{
	EvtAggr::unsubscribe<UiCode>( [&](UiCode &c){ handleEvtCode(c); } );
	EvtAggr::unsubscribe<NoiseInfoRequest*>( [&](NoiseInfoRequest* &n){ n->setInfo(octaves, frequency, persistence, freqDiv); } );

	open_simplex_noise_free(context);
}

void OpenSimplexNoise::handleEvtCode(UiCode &c)
{
	switch(c.code)
	{
		case UIEVT_FREQINCREASE:
			frequency += 0.0002;
		break;

		case UIEVT_FREQDECREASE:
			frequency -= 0.0002;
		break;

		case UIEVT_PERSINCREASE:
			persistence += 0.02;
		break;

		case UIEVT_PERSDECREASE:
			persistence -= 0.02;
		break;

		case UIEVT_FDIVINCREASE:
			freqDiv += 0.02;
		break;

		case UIEVT_FDIVDECREASE:
			freqDiv -= 0.02;
		break;
	}

	switch(c.code)
	{
		case UIEVT_FREQINCREASE:
		case UIEVT_FREQDECREASE:
		case UIEVT_PERSINCREASE:
		case UIEVT_PERSDECREASE:
		case UIEVT_FDIVINCREASE:
		case UIEVT_FDIVDECREASE:
			EvtAggr::publish<UiCode>(UiCode(UIEVT_NOISEINFOUPDATED));
		break;

	}

}

void OpenSimplexNoise::setMap(Map *m)
{
	map = m;
	reset();
}

void OpenSimplexNoise::reset()
{
	state = running;
	doneIts = 0;
	totalIts = map->getMapHeight();
	nowX = 0;
	nowY = 0;

	open_simplex_noise(rand(), &context);
}

int OpenSimplexNoise::getPercentComplete()
{
	return 100 * ((float)doneIts / totalIts);
}

void OpenSimplexNoise::runOnce()
{
	if(state != done)
	{
		if(doneIts == 0)
		{
			map->setHighestH(0);
			map->setLowestH(MAX_H);
			map->setSeaLevel(SEA_LEVEL);
		}

		if(nowY < map->getMapHeight()) // uma linha por chamada
		{
			while(nowX < map->getMapWidth())
			{
				double value = 0;
				double amp = 1, maxAmp = 0;
				double f = frequency;

				//*
				double pi = 3.14159265359;

				int x1 = 0, x2 = map->getMapWidth();
				double s = (double)nowX / map->getMapWidth();
				double dx = x2 - x1;
				
				/*
				int y1 = 0, y2 = map->getMapHeight();
				double t = (double)nowY / map->getMapHeight();
				double dy = y2 - y1;//*/

				double nx = x1 + cos(s * 2 * pi) * dx / (2 * pi);
				//double ny = y1 + cos(t * 2 * pi) * dy / (2 * pi);
				double nz = x1 + sin(s * 2 * pi) * dx / (2 * pi);
				//double nw = y1 + sin(t * 2 * pi) * dy / (2 * pi);//*/

				for(int o = 0; o < octaves; o++) // fractal Brownian motion
				{
					//value += open_simplex_noise2(context, (double) nowX * f, (double) nowY * f) * amp;

					// círculo na diagonal
					//value += open_simplex_noise3(context, (double) nx * f, nowY * f, (double) nz * f) * amp;

					// círculo na diagonal da 4a e linha na outra diagonal perpendicular da 4a
					value += open_simplex_noise4(context, (double) nx * f, nowY * f, (double) nz * f, 0) * amp;
					
					// círculo nas duas diagonais perpendiculares da 4a
					//value += open_simplex_noise4(context, (double) nx * f, (double) ny * f, (double) nz * f, (double) nw * f) * amp;//*/

					maxAmp += amp;
					amp *= persistence;
					f *= freqDiv; // divisão de numero entre 0~1
				}

				value /= maxAmp;

				value *= MAX_H;
				map->setH(nowX, nowY, value);

				if(value > map->getHighestH())
					map->setHighestH(value);

				if(value < map->getLowestH())
					map->setLowestH(value);

				nowX++;
			}

			nowX = 0;
			nowY++;

			doneIts++;
			checkIfFinished();
		}
	}
}

void OpenSimplexNoise::checkIfFinished()
{
	if(doneIts == totalIts && state == running)
	{
		map->normalize(MAX_H);
//*
		if(!alreadySaved) // SALVAR UMA VEZ RESULTADO EM TGA
		{
			unsigned char *imageData;
			imageData = (unsigned char*)malloc(sizeof(unsigned char) * map->getMapWidth() * map->getMapHeight());

			for(int y = 0; y < map->getMapHeight(); y++)
				for(int x = 0; x < map->getMapWidth(); x++)
				{
					if(map->getH(x, y) <= map->getSeaLevel())
						imageData[(map->getMapHeight() - 1 - y) * map->getMapWidth() + x] = 0;//(unsigned char)(((float)(map->getSeaLevel() - 1) / MAX_H) * 256.0);

					else
						imageData[(map->getMapHeight() - 1 - y) * map->getMapWidth() + x] = (unsigned char)((int)((map->getH(x, y) - map->getSeaLevel()) / (float)(MAX_H - map->getSeaLevel()) * 255.0)); //(unsigned char)((int)(((float)map->getH(x, y) / MAX_H) * 256.0));
				}

			tgaSave("t.tga", map->getMapWidth(), map->getMapHeight(), 8, imageData);

			alreadySaved = true;
		}//*/

		state = done;
	}
}

// função para salvar em tga, adaptada de "Antonio Ramires Fernandes ajbrf@yahoo.com" - http://www.lighthouse3d.com/opengl/terrain/index.php3?tgalib
int OpenSimplexNoise::tgaSave(char const	*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData) {

	unsigned char cGarbage = 0, type,mode,aux;
	short int iGarbage = 0;
	int i;
	FILE *file;

// open file and check for errors
	file = fopen(filename, "wb");
	if (file == NULL) {
		return(-1);
	}

// compute image type: 2 for RGB(A), 3 for greyscale
	mode = pixelDepth / 8;
	if ((pixelDepth == 24) || (pixelDepth == 32))
		type = 2;
	else
		type = 3;

// write the header
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	fwrite(&type, sizeof(unsigned char), 1, file);

	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);

	fwrite(&width, sizeof(short int), 1, file);
	fwrite(&height, sizeof(short int), 1, file);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, file);

	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

// convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
	for (i=0; i < width * height * mode ; i+= mode) {
		aux = imageData[i];
		imageData[i] = imageData[i+2];
		imageData[i+2] = aux;
	}

// save the image data
	fwrite(imageData, sizeof(unsigned char), width * height * mode, file);
	fclose(file);
// release the memory
	free(imageData);

	return(0);
}
