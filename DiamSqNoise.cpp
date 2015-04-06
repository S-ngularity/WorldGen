#include "DiamSqNoise.h"

#include "Map.h"

#include <iostream>
#include <cmath>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace std;

DiamSqNoise::DiamSqNoise(Map &theMap) : map(theMap)
{
	totalIts = (int)(log2(MAPWIDTH));
	doneIts = 0;
	percentComplete = 0;
	highestH = 0;
	alreadySaved = false;
	state = running;

	sideLength = MAPWIDTH - 1;
	randRange = MAX_H / 2;

	srand(time(NULL));
}

int DiamSqNoise::getPercentComplete()
{
	return percentComplete;
}

bool DiamSqNoise::isDone()
{
	return state == done ? true : false;
}

int DiamSqNoise::getHighestH()
{
	return highestH;
}

void DiamSqNoise::runOnce()
{
/*
The diamond step: Taking a square of four points, 
generate a random value at the square midpoint, 
where the two diagonals meet. The midpoint value 
is calculated by averaging the four corner values, 
plus a random amount. This gives you diamonds when 
you have multiple squares arranged in a grid.

The square step: Taking each diamond of four points, 
generate a random value at the center of the diamond. 
Calculate the midpoint value by averaging the corner 
values, plus a random amount generated in the same 
range as used for the diamond step. This gives you 
squares again.
*/
	if(MAPWIDTH != MAPHEIGHT || log2(MAPHEIGHT-1) - (int)log2(MAPHEIGHT-1) != 0.0)
	{
		cout << endl << "INVALID MAP SIZE" << endl;
		state = done;
		return;
	}

	else if(state != done)
	{
		if(doneIts == 0)
		{
			map.Tile(0, 0).setH(MAX_H/2);//rand() % (MAX_H + 1));
			map.Tile(0, MAPHEIGHT - 1).setH(MAX_H/2);//rand() % (MAX_H + 1));
			map.Tile(MAPWIDTH - 1, 0).setH(MAX_H/2);//rand() % (MAX_H + 1));
			map.Tile(MAPWIDTH - 1, MAPHEIGHT - 1).setH(MAX_H/2);//rand() % (MAX_H + 1));

			randRange = MAX_H / 2; // REPEATING FOR CLARITY WHILE DEBUGGING
			sideLength = MAPWIDTH - 1;

			//cout << "--- " << map.Tile(0, 0).getH() << " " << map.Tile(0, MAPHEIGHT - 1).getH() << " " << map.Tile(MAPWIDTH - 1, 0).getH() << " " << map.Tile(MAPWIDTH - 1, MAPHEIGHT - 1).getH() << " ---" << endl;
		}

		if(sideLength > 1)
		{
			//cout << "randRage: " << randRange << endl;
			
			squareStep();
			
			diamondStep();
			
			sideLength /= 2;
			randRange *= 0.7;//pow(2, -0.5);
			if(randRange < 1)
				randRange = 1;

			doneIts++;

			updatePercent();
			checkIfFinished();
		}

		else
		{
			state = done;
		}
	}
}

void DiamSqNoise::squareStep()
{
	for(int y = 0; y < MAPHEIGHT - 1; y += sideLength)
		for(int x = 0; x < MAPWIDTH - 1; x += sideLength)
		{
			// average of corner values
			int average = (	map.Tile(x, y).getH() +
							map.Tile(x + sideLength, y).getH() +
							map.Tile(x, y + sideLength).getH() +
							map.Tile(x + sideLength, y + sideLength).getH()) / 4;

			// 2*randRange - randRange pra ficar entre -randRange e +randRange
			int d = ((rand() % (randRange * 2)) - randRange);
			// center of the square is average + random factor
			map.Tile(x + sideLength / 2,
					 y + sideLength / 2).setH(	average +
												d);
		}
}

void DiamSqNoise::diamondStep()
{
	int halfLength = sideLength / 2;

	for(int y = 0; y < MAPHEIGHT; y += halfLength)
		for(int x = (y + halfLength) % sideLength; x < MAPWIDTH - 1; x += sideLength)
		{
			/*
			int A;// = map.Tile(x - halfLength, y).getH(); // left of center
			int B;// = map.Tile(x + halfLength, y).getH(); // right of center
			int C;// = map.Tile(x, y - halfLength).getH(); // above center
			int D;// = map.Tile(x, y + halfLength).getH(); // below center

			int sum = 0, quantity = 0;

			if(x - halfLength >= 0)
			{
				A = map.Tile(x - halfLength, y).getH(); // left of center
				sum += A;
				quantity++;
			}else{A = map.Tile(x + halfLength, y).getH();} // mirror, reusing the one inside

			if(x + halfLength < MAPWIDTH)
			{
				B = map.Tile(x + halfLength, y).getH(); // right of center
				sum += B;
				quantity++;
			}else{B = map.Tile(x - halfLength, y).getH();}

			if(y - halfLength >= 0)
			{
				C = map.Tile(x, y - halfLength).getH(); // above center
				sum += C;
				quantity++;
			}else{C = map.Tile(x, y + halfLength).getH();}

			if(y + halfLength < MAPHEIGHT)
			{
				D = map.Tile(x, y + halfLength).getH(); // below center
				sum += D;
				quantity++;
			}else{D = map.Tile(x, y - halfLength).getH();}

			// average of corner values
			int average = (A+B+C+D)/4;
			//int average = sum / quantity;
			//*/

			int average = (	map.Tile((x - halfLength + MAPWIDTH-1) % (MAPWIDTH-1), y).getH() + // left of center
							map.Tile(x + halfLength % (MAPWIDTH-1), y).getH() + // right of center
							map.Tile(x, (y - halfLength + MAPHEIGHT-1) % (MAPHEIGHT-1)).getH() + // above center
							map.Tile(x, (y + halfLength) % (MAPHEIGHT-1)).getH()) / 4; // below center

			average = average + ((rand() % (randRange * 2)) - randRange);

			map.Tile(x, y).setH(average);

			if(x == 0)
				map.Tile(MAPWIDTH - 1, y).setH(average);
			
			//if(y == 0)
			//	map.Tile(x, MAPHEIGHT - 1).setH(average);
		}
}

void DiamSqNoise::updatePercent()
{
	if((int)(100 * ((float)doneIts / totalIts)) > percentComplete)
		percentComplete = 100 * ((float)doneIts / totalIts);
}

void DiamSqNoise::checkIfFinished()
{
	if(doneIts == totalIts && state == running)
	{
		highestH = 0;

		for(int y = 0; y < MAPHEIGHT; y++)
			for(int x = 0; x < MAPWIDTH; x++)
				if(map.Tile(x, y).getH() > highestH)
					highestH = map.Tile(x, y).getH();
/*
		if(!alreadySaved) // SALVAR UMA VEZ RESULTADO EM TGA
		{
			unsigned char *imageData;
			imageData = (unsigned char*)malloc(sizeof(unsigned char) * MAPWIDTH * MAPHEIGHT);

			for(int y = 0; y < MAPHEIGHT; y++)
				for(int x = 0; x < MAPWIDTH; x++)
				{
					if(map.Tile(x, y).getH() <= SEA_LEVEL)
						imageData[(MAPHEIGHT - 1 - y) * MAPWIDTH + x] = 0;//(unsigned char)(((float)(SEA_LEVEL - 1) / MAX_H) * 256.0);

					else
						imageData[(MAPHEIGHT - 1 - y) * MAPWIDTH + x] = (unsigned char)((int)((map.Tile(x, y).getH() - SEA_LEVEL) / (float)(MAX_H - SEA_LEVEL) * 255.0)); //(unsigned char)((int)(((float)map.Tile(x, y).getH() / MAX_H) * 256.0));
				}

			tgaSave("t.tga", MAPWIDTH, MAPHEIGHT, 8, imageData);

			alreadySaved = true;
		}
//*/
		//cout << endl << "DONE" << endl;
		//state = done;
	}
}

// função para salvar em tga, adaptada de "Antonio Ramires Fernandes ajbrf@yahoo.com" - http://www.lighthouse3d.com/opengl/terrain/index.php3?tgalib
int DiamSqNoise::tgaSave(char const	*filename, 
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