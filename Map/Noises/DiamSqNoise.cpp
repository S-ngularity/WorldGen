#include "DiamSqNoise.h"
#include "Map/Map.h"

#include <iostream>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;


#define TEMP_MAX_H 5000
#define RAND_RANGE_MULTPLIER 0.65

DiamSqNoise::DiamSqNoise(Map *theMap) : 
	Noise("Diamond Square")
{
	//srand(time(NULL));

	map = theMap;
	alreadySaved = false;

	reset();
}

void DiamSqNoise::setMap(Map *m)
{
	map = m;
	reset();
}

void DiamSqNoise::reset()
{
	state = running;
	doneIts = 0;
	totalIts = (int)(log2(map->getMapWidth()));

	sideLength = map->getMapWidth() - 1;
	randRange = TEMP_MAX_H / 2;

	if(map->getMapWidth() != map->getMapHeight() || log2(map->getMapHeight()-1) - (int)log2(map->getMapHeight()-1) != 0.0)
	{
		cout << endl << "INVALID MAP SIZE" << endl;
		state = done;
	}
}

int DiamSqNoise::getPercentComplete()
{
	return 100 * ((float)doneIts / totalIts);
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
	if(state != done)
	{
		if(doneIts == 0)
		{
			map->setHighestH(0);
			map->setLowestH(TEMP_MAX_H);

			map->setSeaLevel(SEA_LEVEL);

			map->setH(0, 0, TEMP_MAX_H/2);//rand() % (TEMP_MAX_H + 1));
			map->setH(0, map->getMapHeight() - 1, TEMP_MAX_H/2);//rand() % (TEMP_MAX_H + 1));
			map->setH(map->getMapWidth() - 1, 0, TEMP_MAX_H/2);//rand() % (TEMP_MAX_H + 1));
			map->setH(map->getMapWidth() - 1, map->getMapHeight() - 1, TEMP_MAX_H/2);//rand() % (TEMP_MAX_H + 1));

			randRange = TEMP_MAX_H / 2; // REPEATING FOR CLARITY WHILE DEBUGGING
			sideLength = map->getMapWidth() - 1;

			//cout << "--- " << map->getH(0, 0) << " " << map->getH(0, map->getMapHeight() - 1) << " " << map->getH(map->getMapWidth() - 1, 0) << " " << map->getH(map->getMapWidth() - 1, map->getMapHeight() - 1) << " ---" << endl;
		}

		if(sideLength > 1)
		{
			//cout << "randRage: " << randRange << endl;
			
			squareStep();
			
			diamondStep();
			
			sideLength /= 2;
			randRange *= RAND_RANGE_MULTPLIER;//pow(2, -0.5);
			if(randRange < 1)
				randRange = 1;

			doneIts++;
			checkIfFinished();
		}
	}
}

void DiamSqNoise::squareStep()
{
	for(int y = 0; y < map->getMapHeight() - 1; y += sideLength)
		for(int x = 0; x < map->getMapWidth() - 1; x += sideLength)
		{
			// average of corner values
			int average = (	map->getH(x, y) +
							map->getH(x + sideLength, y) +
							map->getH(x, y + sideLength) +
							map->getH(x + sideLength, y + sideLength)) / 4;

			// 2*randRange - randRange so the random factor is between -randRange e +randRange
			average = average + ((rand() % (randRange * 2)) - randRange);

			// center of the square is average + random factor
			map->setH(x + sideLength / 2, y + sideLength / 2, average);

			if(average > map->getHighestH())
				map->setHighestH(average);

			if(average < map->getLowestH())
				map->setLowestH(average);
		}
}

void DiamSqNoise::diamondStep()
{
	int halfLength = sideLength / 2;

	for(int y = 0; y < map->getMapHeight(); y += halfLength)
		for(int x = (y + halfLength) % sideLength; x < map->getMapWidth() - 1; x += sideLength)
		{
			//*
			int A = map->getH(((x - halfLength) + map->getMapWidth()-1) % (map->getMapWidth()-1), y);// = map->getH(x - halfLength, y); // left of center
			int B = map->getH((x + halfLength) % (map->getMapWidth()-1), y);// = map->getH(x + halfLength, y); // right of center
			int C;// = map->getH(x, y - halfLength); // above center
			int D;// = map->getH(x, y + halfLength); // below center

			/*sum = 0, quantity = 0;

			if(x - halfLength >= 0)
			{
				A = map->getH(x - halfLength, y); // left of center
				sum += A;
				quantity++;
			}else{A = map->getH(x + halfLength, y);} // mirror, reusing the one inside

			if(x + halfLength < map->getMapWidth())
			{
				B = map->getH(x + halfLength, y); // right of center
				sum += B;
				quantity++;
			}else{B = map->getH(x - halfLength, y);}
			*/
			if(y - halfLength >= 0)
			{
				C = map->getH(x, y - halfLength); // above center
				//sum += C;
				//quantity++;
			}
			
			else
				C = map->getH(x, y + halfLength);

			if(y + halfLength < map->getMapHeight())
			{
				D = map->getH(x, y + halfLength); // below center
				//sum += D;
				//quantity++;
			}

			else
				D = map->getH(x, y - halfLength);

			// average of corner values
			int average = (A+B+C+D)/4;
			//int average = sum / quantity;
			//*/

			average = average + ((rand() % (randRange * 2)) - randRange);

			map->setH(x, y, average);

			if(x == 0)
				map->setH(map->getMapWidth() - 1, y, average);
			
			//if(y == 0)
			//	map->setH(x, map->getMapHeight() - 1, average);

			if(average > map->getHighestH())
				map->setHighestH(average);

			if(average < map->getLowestH())
				map->setLowestH(average);
		}
}

void DiamSqNoise::checkIfFinished()
{
	if(doneIts == totalIts && state == running)
	{
		map->normalize(MAX_H);
/*
		if(!alreadySaved) // SALVAR UMA VEZ RESULTADO EM TGA
		{
			unsigned char *imageData;
			imageData = (unsigned char*)malloc(sizeof(unsigned char) * map->getMapWidth() * map->getMapHeight());

			for(int y = 0; y < map->getMapHeight(); y++)
				for(int x = 0; x < map->getMapWidth(); x++)
				{
					if(map->getH(x, y) <= SEA_LEVEL)
						imageData[(map->getMapHeight() - 1 - y) * map->getMapWidth() + x] = 0;//(unsigned char)(((float)(SEA_LEVEL - 1) / MAX_H) * 256.0);

					else
						imageData[(map->getMapHeight() - 1 - y) * map->getMapWidth() + x] = (unsigned char)((int)((map->getH(x, y) - SEA_LEVEL) / (float)(MAX_H - SEA_LEVEL) * 255.0)); //(unsigned char)((int)(((float)map->getH(x, y) / MAX_H) * 256.0));
				}

			tgaSave("t.tga", map->getMapWidth(), map->getMapHeight(), 8, imageData);

			alreadySaved = true;
		}//*/

		state = done;
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
