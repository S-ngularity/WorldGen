#include "Map/Map.h"

Map::Map(int width, int height) : 
	mapWidth(width), 
	mapHeight(height), 
	map(mapWidth * mapHeight, INIT_H), 
	highestH(INIT_H), 
	lowestH(INIT_H), 
	seaLevel(SEA_LEVEL)
{}

int Map::getH(int x, int y) const
{
	if(isPosInsideWrap(x, y))
	{
		if(x < 0)
			x = mapWidth + x % mapWidth;

		else if(x >= mapWidth)
			x = x % mapWidth;
	}

/*if(isPosInsideWrap(x, y)) // WRAP IN Y (mudar isPosInsideWrap para só true)
{
	if(y < 0)
		y = mapHeight + y % mapHeight;

	else if(y >= mapHeight)
		y = y % mapHeight;
}//*/

	if(isPosInsideNoWrap(x, y))
		return map[x + y*mapWidth];

	else
		return map[0];
}

void Map::setH(int x, int y, int newHeight)
{
	map[x + y*mapWidth] = newHeight;
}

void Map::normalize(int maxH)
{
	for(int y = 0; y < mapHeight; y++)
		for(int x = 0; x < mapWidth; x++)
		{
			map[x + y*mapWidth] = ((map[x + y*mapWidth] - lowestH) / (float)(highestH - lowestH)) * maxH;
		}

	setHighestH(maxH); // there will be always a highest point normalized from the original highest point
	setLowestH(0);
}

bool Map::isPosInsideWrap(int x, int y) const
{
	if(y >= 0 && y < mapHeight)
		return true;

	else
		return false;
}

bool Map::isPosInsideNoWrap(int x, int y) const
{
	if(x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
		return true;

	else
		return false;
}

int Map::getMapHeight() const
{
	return mapHeight;
}

int Map::getMapWidth() const
{
	return mapWidth;
}

int Map::getHighestH() const
{
	return highestH;
}

void Map::setHighestH(int h)
{
	highestH = h;
}

int Map::getLowestH() const
{
	return lowestH;
}

void Map::setLowestH(int h)
{
	lowestH = h;
}

int Map::getSeaLevel() const
{
	return seaLevel;
}

void Map::setSeaLevel(int lvl)
{
	seaLevel = lvl;
}

void Map::increaseSeaLevel()
{
	seaLevel++;
}

void Map::decreaseSeaLevel()
{
	seaLevel--;
}
