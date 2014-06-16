#include <windows.h>
#include <stdio.h>
#include "map.h"

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // seleciona o console sendo usado

Pos insertSeed(int &h, Tile m[MAPSIZE][MAPSIZE])
{
	Pos temp;

	/*if(numMedTerrain > 0)
	{
		do{
			temp.x = rand() % MAPSIZE;
			temp.y = rand() % MAPSIZE;
			h = 3;
		} while(h < m[temp.x][temp.y].h);

		numMedTerrain--;
	}

	else
	{*/
		do{
			temp.x = rand() % MAPSIZE;
			temp.y = rand() % MAPSIZE;
			h = rand() % MAX_H;
		} while(h < m[temp.x][temp.y].h);
	//}

	m[temp.x][temp.y].h = h;
	m[temp.x][temp.y].chance = setBaseChance(m, temp.x, temp.y);
	m[temp.x][temp.y].pred.x = NULO;
	m[temp.x][temp.y].pred.y = NULO;
	m[temp.x][temp.y].isSeed = 1;
	m[temp.x][temp.y].skip = 1;
	m[temp.x][temp.y].printPred = 0;

	return temp;
}

int setBaseChance(Tile m[MAPSIZE][MAPSIZE], int x, int y)
{
	int chance;

	/*
	if(m[x][y].h == 6)
		chance = 50;

	else if(m[x][y].h == 5)
		chance = 60;

	else if(m[x][y].h == 4)
		chance = 70;

	else if(m[x][y].h == 3)
		chance = 80;	

	else if(m[x][y].h == 2)
		chance = 10;

	else if(m[x][y].h == 1)
		chance = 10;

	else if(m[x][y].h == 0)
		chance = 0;

	else
		chance = 50;
	//*/

	chance = rand() % 101;

	return chance;
}

int lowerChance(Tile m[MAPSIZE][MAPSIZE], int oldX, int oldY)
{
	int chance;

	/*
	if(m[oldX][oldY].h == 6)
		chance = (int) m[oldX][oldY].chance * 0.40 * MULTIPLIER;

	else if(m[oldX][oldY].h == 5)
		chance = (int) m[oldX][oldY].chance * 0.50 * MULTIPLIER;

	else if(m[oldX][oldY].h == 4)
		chance = (int) m[oldX][oldY].chance * 0.60 * MULTIPLIER;

	else if(m[oldX][oldY].h == 3)
		chance = (int) m[oldX][oldY].chance * 0.70 * MULTIPLIER;

	else if(m[oldX][oldY].h == 2)
		chance = (int) m[oldX][oldY].chance * 0.1 * MULTIPLIER;

	else if(m[oldX][oldY].h == 1)
		chance = (int) m[oldX][oldY].chance * 0.1 * MULTIPLIER;

	else if(m[oldX][oldY].h == 0)
		chance = (int) m[oldX][oldY].chance * 0.0 * MULTIPLIER;

	else
		chance = (int) m[oldX][oldY].chance * 0.5 * MULTIPLIER;
	//*/

	chance =  m[oldX][oldY].chance * ((rand() % 11) / 10) * MULTIPLIER;

	return chance;
}

void printMap(Tile m[MAPSIZE][MAPSIZE])
{
	int i, j;

	printf("   ");

	for(i = 0; i < (MAPSIZE / 10); i++)
		for(j = 0; j < 10; j++)
			printf("%d", i);

	printf("\n   ");

	for(i = 0; i < (MAPSIZE / 10); i++)
		for(j = 0; j < 10; j++)
			printf("%d", j);

	printf("\n");

	for(j = 0; j < MAPSIZE; j++)
	{
		printf("%3d", j);
		for(i = 0; i < MAPSIZE; i++)
			{
				if(m[i][j].printPred == 1)
					printPred(m[i][j].h);

				//else if(m[i][j].isSeed == 1)
				//	printSeed(m[i][j].h);

				else
					printColor(m[i][j].h);
			}

		printf("\n");
	}
}

void printMapWithPreds(Tile m[MAPSIZE][MAPSIZE])
{
	int aux1, aux2, i, j;

	scanf("%d %d", &aux1, &aux2);

	while(aux1 > -1 && aux2 > -1)
	{
		for(i = 0; i < MAPSIZE; i++)
			for(j = 0; j < MAPSIZE; j++)
				m[i][j].printPred = 0;

		while(m[aux1][aux2].pred.x != NULO || m[aux1][aux2].pred.y != NULO)
		{
			m[aux1][aux2].printPred = 1;
			aux1 = m[aux1][aux2].pred.x;
			aux2 = m[aux1][aux2].pred.y;
		}

		printMap(m);

		scanf("%d %d", &aux1, &aux2);
	}
}

void printPred(int n)
{
	switch(n)
	{
		case 0 :
			SetConsoleTextAttribute(hConsole, 1);
			printf("%d", n);
			break;

		case 1 :
			SetConsoleTextAttribute(hConsole, 11);
			printf("%d", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 15);
			printf("%d", n);
			break;
		
		case 3 :
			SetConsoleTextAttribute(hConsole, 2);
			printf("%d", n);
			break;
		
		case 4 :
			SetConsoleTextAttribute(hConsole, 8);
			printf("%d", n);
			break;
		
		case 5 :
			SetConsoleTextAttribute(hConsole, 7);
			printf("%d", n);
			break;
		
		case 6 :
			SetConsoleTextAttribute(hConsole, 127);
			printf("%d", n);
			break;
		default:
			SetConsoleTextAttribute(hConsole, 7);		
			printf("%d", n);
			break;
	}

	SetConsoleTextAttribute(hConsole, 7);
}

void printSeed(int n)
{
	switch(n)
	{
		case 0 :
			SetConsoleTextAttribute(hConsole, 16);
			printf("%d", n);
			break;

		case 1 :
			SetConsoleTextAttribute(hConsole, 48);
			printf("%d", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 96);
			printf("%d", n);
			break;
		
		case 3 :
			SetConsoleTextAttribute(hConsole, 32);
			printf("%d", n);
			break;
		
		case 4 :
			SetConsoleTextAttribute(hConsole, 128);
			printf("%d", n);
			break;
		
		case 5 :
			SetConsoleTextAttribute(hConsole, 112);
			printf("%d", n);
			break;
		
		case 6 :
			SetConsoleTextAttribute(hConsole, 240);
			printf("%d", n);
			break;
		default:
			SetConsoleTextAttribute(hConsole, 7);		
			printf("%d", n);
			break;
	}

	SetConsoleTextAttribute(hConsole, 7);
}

///*
void printColor(int n)
{
	int placeholder = n % 10;

	switch(placeholder)
	{
		case 0 :
			SetConsoleTextAttribute(hConsole, 17);
			printf("%d", placeholder);
			break;

		case 1 :
			SetConsoleTextAttribute(hConsole, 51);
			printf("%d", placeholder);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 102);
			printf("%d", placeholder);
			break;
		
		case 3 :
			SetConsoleTextAttribute(hConsole, 34);
			printf("%d", placeholder);
			break;
		
		case 4 :
			SetConsoleTextAttribute(hConsole, 136);
			printf("%d", placeholder);
			break;
		
		case 5 :
			SetConsoleTextAttribute(hConsole, 119);
			printf("%d", placeholder);
			break;
		
		case 6 :
			SetConsoleTextAttribute(hConsole, 255);
			printf("%d", placeholder);
			break;
		default:
			SetConsoleTextAttribute(hConsole, 7);		
			printf("%d", placeholder);
			break;
	}

	SetConsoleTextAttribute(hConsole, 7);
}
//*/

/*
void printColor(int n)
{
	switch(n)
	{
		case 0 :
			SetConsoleTextAttribute(hConsole, 1);
			printf("%d", n);
			break;

		case 1 :
			SetConsoleTextAttribute(hConsole, 11);
			printf("%d", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 15);
			printf("%d", n);
			break;
		
		case 3 :
			SetConsoleTextAttribute(hConsole, 2);
			printf("%d", n);
			break;
		
		case 4 :
			SetConsoleTextAttribute(hConsole, 8);
			printf("%d", n);
			break;
		
		case 5 :
			SetConsoleTextAttribute(hConsole, 7);
			printf("%d", n);
			break;
		
		case 6 :
			SetConsoleTextAttribute(hConsole, 127);
			printf("%d", n);
			break;
		default:
			SetConsoleTextAttribute(hConsole, 7);		
			printf("%d", n);
			break;
	}

	SetConsoleTextAttribute(hConsole, 7);
}
//*/