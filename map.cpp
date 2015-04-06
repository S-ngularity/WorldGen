#include "Map.h"

#include "Pos.h"

Map::Map()
{
	map = new MapTile*[MAPWIDTH];

	for(int i = 0; i < MAPWIDTH; i++)
		map[i] = new MapTile[MAPHEIGHT];

	for(int y = 0; y < MAPHEIGHT; y++)
		for(int x = 0; x < MAPWIDTH; x++)
			Tile(x, y).setH(INIT_H);
}

Map::~Map()
{
	for(int i = 0; i < MAPWIDTH; i++)
		delete [] map[i];

	delete [] map;
}

MapTile& Map::Tile(Pos p)
{
	return Tile(p.getX(), p.getY());
}

MapTile& Map::Tile(int x, int y)
{
	if(isPosInsideWrap(x, y))
	{
		if(x < 0)
			x = MAPWIDTH + x % MAPWIDTH;

		else if(x >= MAPWIDTH)
			x = x % MAPWIDTH;
	}

/*if(isPosInsideWrap(x, y)) // WRAP IN Y (mudar isPosInsideWrap para só true)
{
	if(y < 0)
		y = MAPHEIGHT + y % MAPHEIGHT;

	else if(y >= MAPHEIGHT)
		y = y % MAPHEIGHT;
}//*/

	if(isPosInsideNoWrap(x, y))
		return map[x][y];

	else
		return map[0][0];
}

bool Map::isPosInsideNoWrap(Pos p)
{
	if(p.getX() >= 0 && p.getY() >= 0 && p.getX() < MAPWIDTH && p.getY() < MAPHEIGHT)
		return true;

	else
		return false;
}	

bool Map::isPosInsideWrap(Pos p)
{
	if(p.getY() >= 0 && p.getY() < MAPHEIGHT)
		return true;

	else
		return false;
}

bool Map::isPosInsideNoWrap(int x, int y)
{
	if(x >= 0 && y >= 0 && x < MAPWIDTH && y < MAPHEIGHT)
		return true;

	else
		return false;
}	

bool Map::isPosInsideWrap(int x, int y)
{
	if(y >= 0 && y < MAPHEIGHT)
		return true;

	else
		return false;
}