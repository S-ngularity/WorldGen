#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"
#include "filaPos.h"

#include <SDL2/SDL.h>

using namespace std;

const int SCREEN_WIDTH = MAPSIZE * 5;
const int SCREEN_HEIGHT = MAPSIZE * 5;

//The window we'll be rendering to
SDL_Window *Window = NULL;

//The window renderer
SDL_Renderer *Renderer = NULL;

//funções SDL
bool SDLStart();
void SDLClose();

int main(int argc, char* args[])
{
	Map map;
	Tile auxTile;
	Pos auxPos;

	Fila filaAtual, filaLower;	// filas de posições

	int iterations, numIts;//iterações de inserção de seeds

	// altura sendo desenhada
	int hAtual;

	//contadores
	int i, j;

	inicializa_fila(&filaAtual);
	inicializa_fila(&filaLower);
	
	/*// seed manual
	int seed;
	cout << "Seed: ";
	cin >> seed;
	cout << endl;

	srand(seed);
	//*/
	srand(time(NULL));

	//scanf("%d", &numMedTerrain);
	cout << "Iterations: ";
	cin >> numIts;
	cout << endl;

	//numIts = 1000;



	for(iterations = 0; iterations < numIts; iterations++)
	{
		auxPos = map.insertSeed();

		hAtual = map.getTile(auxPos).h;

		insere_fila(&filaAtual, auxPos);

		//for(i = 0; i < MAPSIZE; i++)
		//		for(j = 0; j < MAPSIZE; j++)
		//			m[i][j].visitado = 0;

		// zera skips
		for(i = 0; i < MAPSIZE; i++)
			for(j = 0; j < MAPSIZE; j++)
			{
				auxTile = map.getTile(j, i);

				if(auxTile.h < hAtual)
				{
					auxTile.skip = false;
					map.setTile(auxTile);
				}
			}

		while(hAtual > 0)
		{
			// checa toda a fila
			while(!fila_vazia(&filaAtual))
			{
				Tile queueTile(remove_fila(&filaAtual)); // posição da fila a ser trabalhada

				// checa posições adjacentes a posição atual da fila e as modifica
				for(i = -1; i <= 1; i++)
					for(j = -1; j <= 1; j++)
					{
						// adjPos é a posição adjacente nesta iteração
						Tile newAdjTile;

						// pula posição se estiver fora do mapa
						try
						{
							newAdjTile = map.getTile(queueTile.pos.x + j, queueTile.pos.y + i);
						}
						catch(bool) { continue; }

						if(hAtual > newAdjTile.h) // adjacente está dentro do mapa e é menor que altura atual
						   //&& m[adjPos.x][adjPos.y].visitado == 0)
						{
							newAdjTile.pred = queueTile.pos; // marca predecessor como posição que foi tirada da fila
							//newAdjTile.visitado = 1;

							// diminui ou não altura da adjacente baseado na chance de manter do atual
							if(rand() % 100 <= queueTile.chance)	// mantem altura e diminui chance dos próximos manterem
							{
								newAdjTile.h = hAtual;
								newAdjTile.chance = map.lowerChance(queueTile.pos);
								newAdjTile.skip = true;

								insere_fila(&filaAtual, newAdjTile.pos);	// coloca tile de mesma altura na fila de altura atual
							}

							else  // insere na fila da próxima altura (diminui altura)
								insere_fila(&filaLower, newAdjTile.pos);

							map.setTile(newAdjTile);
						}
					} // para todos os adjacentes do membro atual da fila
			} // enquanto filaAtual não estiver vazia

			hAtual--;

			// esvazia próxima fila colocando membros não repetidos na fila atual e setando altura/chance de manter
			while(!fila_vazia(&filaLower))
			{
				auxTile = map.getTile(remove_fila(&filaLower));

				if(auxTile.skip == false)
				{
					auxTile.skip = true;
					auxTile.h = hAtual;
					map.setBaseChance(auxTile.pos);
					
					insere_fila(&filaAtual, auxTile.pos);

					map.setTile(auxTile);
				}
			}

			// se chegou no fim das inserções dessa seed, zera fila atual
			if(hAtual == 0)
			{
				while(!fila_vazia(&filaAtual))
				{
					remove_fila(&filaAtual);
				}
			}
		} // enquanto filaAtual não estiver vazia
	} // para todas as iterações

	//printMap(m);

	/*
	for(i = 0; i < MAPSIZE; i++)
		for(j = 0; j < MAPSIZE; j++)
		{
			if(m[i][j].h < 10)
				m[i][j].h = 0;
		}
	*/

	if(!SDLStart())
	{
		printf("ERRO AO INICIALIZAR SDL");

		return -1;
	}

	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
	SDL_RenderClear(Renderer);

	int contJ = 0, contI = 0;

	//Draw vertical line of yellow dots
	for(i = 0; i < MAPSIZE; contI++)
	{
		if(contI == 5)
		{
			contI = 0;
			i++;
		}

		for(j = 0; j < MAPSIZE; contJ++)
		{
			if(contJ == 5)
			{
				contJ = 0;
				j++;
			}

			int hColor = map.getTile(j, i).h;

			SDL_SetRenderDrawColor(Renderer, hColor * 30, hColor * 30, hColor * 30, 255);
			SDL_RenderDrawPoint(Renderer, j*5 + contJ, i*5 + contI);
		}
	}

	//Update screen
	SDL_RenderPresent(Renderer);

	//Event handler
	SDL_Event e;

	bool quit;

	//While application is running
	while(!quit)
	{
		//Handle events on queue
		SDL_PollEvent(&e);

		if(e.type == SDL_QUIT)
			quit = true;
	}

	SDLClose();

//	printMapWithPreds(m);
/*
	// imprime erros de quando a diferença entre tiles adjacentes é maior que 1
	for(i = 0; i < MAPSIZE; i++)
		for(j = 0; j < MAPSIZE; j++)
		{
			for(k = -1; k <= 1; k++)
				for(l = -1; l <= 1; l++)
				{
					tempPos.x = i + k;
					tempPos.y = j + l;

					if((tempPos.x >= 0 && tempPos.y >= 0 && tempPos.x < MAPSIZE && tempPos.y < MAPSIZE))
					{
						if((m[tempPos.x][tempPos.y].h - m[i][j].h) > 1 || (m[tempPos.x][tempPos.y].h - m[i][j].h) < -1)
						{
							printf("ERRO EM %d %d\n", j, i);
						}
					}
				}

		}
*/
	return 0;
}


bool SDLStart()
{
	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

		return false;
	}

	//if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	//{
	//	printf("Warning: Linear texture filtering not enabled!");
	//}

	//Create window
	Window = SDL_CreateWindow("WorldGen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(Window == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}
	
	//Create renderer for window
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	if(Renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);

	return true;
}

void SDLClose()
{
	//Destroy window	
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	Window = NULL;
	Renderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}