#include <stdlib.h>
#include "MapTile.h"

#include "Pos.h" // pred

#define NULO -1

MapTile::MapTile() : pred(NULO, NULO)
{
	h = 0;
	chance = 0;
	isSeed = false;
	skip = false;
	error = false;
}

// h
int MapTile::getH()
{
	return h;
}

void MapTile::setH(int newH)
{
	h = newH;
}


// chance
int MapTile::getChance()
{
	return chance;
}

void MapTile::setBaseChance()
{
	chance = 50 + rand() % 51;
}

void MapTile::lowerChance(MapTile prevTile)
{
	chance = (float)prevTile.getChance() * (float)((float)(rand() % 101) / (float)100) * MULTIPLIER;
	//chance = 0;
}

// pred
Pos MapTile::getPred()
{
	return pred;
}

void MapTile::setPred(Pos newPred)
{
	pred = newPred;
}

void MapTile::setPred(int predX, int predY)
{
	pred.setPos(predX, predY);
}

// isSeed
bool MapTile::getIsSeed()
{
	return isSeed;
}

void MapTile::setIsSeed(bool newIsSeed)
{
	isSeed = newIsSeed;
}


// skip
bool MapTile::getSkip()
{
	return skip;
}

void MapTile::setSkip(bool newSkip)
{
	skip = newSkip;
}

bool MapTile::getError()
{
	return error;
}

void MapTile::setError(bool newError)
{
	error = newError;
}