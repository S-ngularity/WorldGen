#include "OpenSimplexNoise.h"

#include <iostream>
#include <cmath>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace std;

OpenSimplexNoise::OpenSimplexNoise(Map &theMap, int oct, double freq, double pers, double fdiv) : map(theMap)
{
	state = running;
	alreadySaved = false;
	doneIts = 0;
	totalIts = map.getMapHeight();
	nowX = 0;
	nowY = 0;
	map.setHighestH(0);
	map.setLowestH(MAX_H);

	srand(time(NULL));

	octaves = oct;
	frequency = freq;
	persistence = pers;
	freqDiv = fdiv;

	open_simplex_noise(rand(), &context);
}

OpenSimplexNoise::~OpenSimplexNoise()
{
	open_simplex_noise_free(context);
}

int OpenSimplexNoise::getPercentComplete()
{
	return 100 * ((float)doneIts / totalIts);
}

void OpenSimplexNoise::runOnce()
{
	if(state != done)
	{
		if(nowY < map.getMapHeight()) // uma linha por chamada
		{
			while(nowX < map.getMapWidth())
			{
				double value = 0;
				double amp = 1, maxAmp = 0;
				double f = frequency;

				//*
				int x1 = 0, x2 = map.getMapWidth(), y1 = 0, y2 = map.getMapHeight();
				double pi = 3.14159265359;

				double s = (double)nowX / map.getMapWidth();
				double t = (double)nowY / map.getMapHeight();
				double dx = x2 - x1;
				double dy = y2 - y1;

				double nx = x1 + cos(s * 2 * pi) * dx / (2 * pi);
				double ny = y1 + (t * 2 * pi) * dy / (2 * pi);
				double nz = x1 + sin(s * 2 * pi) * dx / (2 * pi);
				double nw = y1 + sin(t * 2 * pi) * dy / (2 * pi);//*/

				for(int o = 0; o < octaves; o++) // fractal Brownian motion
				{
					value += open_simplex_noise3(context, (double) nx * f, (double) nz * f, (double) ny * f) * amp;

					//*//
					//value += open_simplex_noise3(context, (double) nowX * f, (double) nowY * f, 0) * amp;

					/*/value += open_simplex_noise4(context, (double) nx * f, 
															(double) ny * f, (double) nz * f, (double) nw * f) * amp;//*/

					maxAmp += amp;
					amp *= persistence;
					f *= freqDiv; // divisão de numero entre 0~1
				}

				value /= maxAmp;

				value *= MAX_H;
				map.Tile(nowX, nowY).setH(value);

				if(value > map.getHighestH())
					map.setHighestH(value);

				if(value < map.getLowestH())
					map.setLowestH(value);

				nowX++;
			}

			//else
			//{
				nowX = 0;
				nowY++;

				doneIts++;
				checkIfFinished();
			//}

		}
	}
}

void OpenSimplexNoise::checkIfFinished()
{
	if(doneIts == totalIts && state == running)
	{
		map.normalize(MAX_H);
/*
		int seaLevel = (map.getHighestH() / 2 ) - 1;
		if(!alreadySaved) // SALVAR UMA VEZ RESULTADO EM TGA
		{
			unsigned char *imageData;
			imageData = (unsigned char*)malloc(sizeof(unsigned char) * map.getMapWidth() * map.getMapHeight());

			for(int y = 0; y < map.getMapHeight(); y++)
				for(int x = 0; x < map.getMapWidth(); x++)
				{
					if(map.Tile(x, y).getH() <= seaLevel)
						imageData[(map.getMapHeight() - 1 - y) * map.getMapWidth() + x] = 0;//(unsigned char)(((float)(seaLevel - 1) / MAX_H) * 256.0);

					else
						imageData[(map.getMapHeight() - 1 - y) * map.getMapWidth() + x] = (unsigned char)((int)((map.Tile(x, y).getH() - seaLevel) / (float)(MAX_H - seaLevel) * 255.0)); //(unsigned char)((int)(((float)map.Tile(x, y).getH() / MAX_H) * 256.0));
				}

			tgaSave("t.tga", map.getMapWidth(), map.getMapHeight(), 8, imageData);

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