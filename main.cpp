#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"
#include "filaPos.h"

#include <SDL2/SDL.h>

using namespace std;

const int SCREEN_WIDTH = MAPSIZE * 4;
const int SCREEN_HEIGHT = MAPSIZE * 4;

//The window we'll be rendering to
SDL_Window *Window = NULL;

//The window renderer
SDL_Renderer *Renderer = NULL;

//funções SDL
bool SDLStart();
void SDLClose();

int main(int argc, char* args[])
{
	Tile m[MAPSIZE][MAPSIZE]; 	// mapa
	Pos atual, temp;			// posição atual retirada da lista e temp pra manipulações
	Fila filaAtual, filaLower;	// filas de posições

	int iterations, numIts;//iterações de inserção de seeds

	// altura sendo desenhada
	int hAtual;

	//contadores
	int i, j;

	// zera todos os tiles
	for(i = 0; i < MAPSIZE; i++)
		for(j = 0; j < MAPSIZE; j++)
		{
				m[i][j].h = 0;
				m[i][j].chance = 0;
				m[i][j].pred.x = NULO;
				m[i][j].pred.y = NULO;
				m[i][j].isSeed = 0;
				m[i][j].skip = 0;
				m[i][j].printPred = 0;
				m[i][j].visitado = 0;
		}

	

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

	if(!SDLStart())
	{
		printf("ERRO AO INICIALIZAR SDL");

		return -1;
	}

	for(iterations = 0; iterations < numIts; iterations++)
	{
		insere_fila(&filaAtual, insertSeed(hAtual, m));

		//for(i = 0; i < MAPSIZE; i++)
		//		for(j = 0; j < MAPSIZE; j++)
		//			m[i][j].visitado = 0;

		// zera skips
		for(i = 0; i < MAPSIZE; i++)
			for(j = 0; j < MAPSIZE; j++)
				if(m[i][j].h < hAtual)
					m[i][j].skip = 0;

		while(hAtual > 0)
		{
			// checa toda a fila
			while(!fila_vazia(&filaAtual))
			{
				atual = remove_fila(&filaAtual); // posição da fila a ser trabalhada

				// checa posições adjacentes a posição atual da fila e as modifica
				for(i = -1; i <= 1; i++)
					for(j = -1; j <= 1; j++)
					{
						// temp é a posição adjacente nesta iteração
						temp.x = atual.x + j;
						temp.y = atual.y + i;

						// adjacente está dentro do mapa e é menor que altura atual
						if(temp.x >= 0 && temp.y >= 0 && temp.x < MAPSIZE && temp.y < MAPSIZE
							&& hAtual > m[temp.x][temp.y].h)
							//&& m[temp.x][temp.y].visitado == 0)
						{
							m[temp.x][temp.y].pred = atual; // marca predecessor como posição que foi tirada da fila
							//m[temp.x][temp.y].visitado = 1;

							// diminui ou não altura da adjacente baseado na chance de manter do atual
							if(rand() % 100 <= m[atual.x][atual.y].chance)	// mantem altura e diminui chance dos próximos manterem
							{
								m[temp.x][temp.y].h = hAtual;
								m[temp.x][temp.y].chance = lowerChance(m, atual.x, atual.y);

								m[temp.x][temp.y].skip = 1;

								insere_fila(&filaAtual, temp);	// coloca tile de mesma altura na fila de altura atual
							}

							else  // insere na fila da próxima altura (diminui altura)
								insere_fila(&filaLower, temp);
						}
					} // para todos os adjacentes do membro atual da fila
			} // enquanto filaAtual não estiver vazia

			hAtual--;

			// esvazia próxima fila colocando membros não repetidos na fila atual e setando altura/chance de manter
			while(!fila_vazia(&filaLower))
			{
				atual = remove_fila(&filaLower);

				if(m[atual.x][atual.y].skip == 0)
				{
					m[atual.x][atual.y].skip = 1;
					m[atual.x][atual.y].h = hAtual;
					m[atual.x][atual.y].chance = setBaseChance(m, atual.x, atual.y);
					insere_fila(&filaAtual, atual);
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

	for(i = 0; i < MAPSIZE; i++)
		for(j = 0; j < MAPSIZE; j++)
		{
			if(m[i][j].h < 10)
				m[i][j].h = 0;
		}

	//Clear screen
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
	SDL_RenderClear(Renderer);

	int contJ = 0, contI = 0;
	//Draw vertical line of yellow dots
	for(i = 0; i < MAPSIZE; contI++)
	{
		if(contI == 4)
		{
			contI = 0;
			i++;
		}

		for(j = 0; j < MAPSIZE; contJ++)
		{
			if(contJ == 4)
			{
				contJ = 0;
				j++;
			}

			SDL_SetRenderDrawColor(Renderer, m[i][j].h * 10, m[i][j].h * 10, m[i][j].h * 10, 255);
			SDL_RenderDrawPoint(Renderer, j*4 + contJ, i*4 + contI);
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
					temp.x = i + k;
					temp.y = j + l;

					if((temp.x >= 0 && temp.y >= 0 && temp.x < MAPSIZE && temp.y < MAPSIZE))
					{
						if((m[temp.x][temp.y].h - m[i][j].h) > 1 || (m[temp.x][temp.y].h - m[i][j].h) < -1)
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