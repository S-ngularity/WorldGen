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
	Pos tempPos(x, y);

	if(isPosInsideWrap(tempPos))
	{
		if(tempPos.getX() < 0)
			tempPos.setPos(MAPWIDTH + tempPos.getX(), tempPos.getY());

		else if(tempPos.getX() >= MAPWIDTH)
			tempPos.setPos(0 + (tempPos.getX() - MAPWIDTH), tempPos.getY());
	}

/*if(isPosInsideWrap(tempPos)) // WRAP IN Y (mudar isPosInsideWrap para só true)
{
	if(tempPos.getY() < 0)
		tempPos.setPos(tempPos.getX(), MAPHEIGHT + tempPos.getY());

	else if(tempPos.getY() >= MAPHEIGHT)
		tempPos.setPos(tempPos.getX(), 0 + (tempPos.getY() - MAPHEIGHT));
}//*/

	if(isPosInsideNoWrap(tempPos))
		return map[tempPos.getX()][tempPos.getY()];

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