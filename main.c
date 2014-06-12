#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define MAPSIZE 85

#define MULTIPLIER 1

#define MAX_H 7

#define NULO -1

// struct de posição
typedef struct pos
{
	int x;
	int y;
} Pos;

// struct de cada tile do mapa
typedef struct tile
{
	int h;
	int chance;
	Pos pred;
	int isSeed;
	int skip;
	int printPred;
	int visitado;
} Tile;

// nó da fila de posições do mapa a serem alteradas
typedef struct no
{
	Pos pos;
	struct no *prox;
} No;

// fila de posições do mapa
typedef struct fila
{
	No *head;
	No *tail;
} Fila;

typedef No* Lista;

// funções de inserção de seeds
Pos insertSeed(Tile m[MAPSIZE][MAPSIZE]);

//set chance de próximos tiles manterem altura
int setBaseChance(Tile m[MAPSIZE][MAPSIZE], int x, int y); // retorna chance base do tile de acordo com altura
int lowerChance(Tile m[MAPSIZE][MAPSIZE], int oldX, int oldY); // retorna nova chance diminuida baseada no tile anterior

//imprimir tile na cor certa (no Windows)
void printMap(Tile m[MAPSIZE][MAPSIZE]);
void printMapWithPreds(Tile m[MAPSIZE][MAPSIZE]);
void printColor(int n);
void printSeed(int n);
void printPred(int n);

//funções de fila
void inicializa_fila(Fila *fila);
void insere_fila(Fila *fila, Pos p);
Pos remove_fila(Fila *fila);
int fila_vazia(Fila *fila);
void esvazia_fila(Fila *fila);

HANDLE hConsole; // console do Windows (pra mudar cores dos tiles quando for imprimir)

int hAtual;
int numMedTerrain = 0;

int main()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // seleciona o console sendo usado

	Tile m[MAPSIZE][MAPSIZE]; 	// mapa
	Pos atual, temp;			// posição atual retirada da lista e temp pra manipulações
	Fila filaAtual, filaLower;	// filas de posições

	int iterations, numIts;//iterações de inserção de seeds

	//contadores
	int i, j, k, l;

	//*// seed manual
	int seed;
	
	printf("Seed: ");
	scanf("%d", &seed);
	srand(seed);
	//*/
	//srand(time(NULL));

	inicializa_fila(&filaAtual);
	inicializa_fila(&filaLower);

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

	//scanf("%d", &numMedTerrain);
	printf("Iterations: ");
	scanf("%d", &numIts);

	for(iterations = 0; iterations < numIts; iterations++)
	{
		insere_fila(&filaAtual, insertSeed(m));

		for(i = 0; i < MAPSIZE; i++)
				for(j = 0; j < MAPSIZE; j++)
					m[i][j].visitado = 0;

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
							&& hAtual > m[temp.x][temp.y].h
							&& m[temp.x][temp.y].visitado == 0)
						{
							m[temp.x][temp.y].pred = atual; // marca predecessor como posição que foi tirada da fila
							m[temp.x][temp.y].visitado = 1;

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

	printMap(m);

//	printMapWithPreds(m);

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

	return 0;
}

Pos insertSeed(Tile m[MAPSIZE][MAPSIZE])
{
	Pos temp;

	if(numMedTerrain > 0)
	{
		do{
			temp.x = rand() % MAPSIZE;
			temp.y = rand() % MAPSIZE;
			hAtual = 3;
		} while(hAtual < m[temp.x][temp.y].h);

		numMedTerrain--;
	}

	else
	{
		do{
			temp.x = rand() % MAPSIZE;
			temp.y = rand() % MAPSIZE;
			hAtual = rand() % MAX_H;
		} while(hAtual < m[temp.x][temp.y].h);
	}

	m[temp.x][temp.y].h = hAtual;
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

	if(m[x][y].h == 6)
		chance = 50;

	else if(m[x][y].h == 5)
		chance = 60;

	else if(m[x][y].h == 4)
		chance = 70;

	else if(m[x][y].h == 3)
		chance = 90;	

	else if(m[x][y].h == 2)
		chance = 10;

	else if(m[x][y].h == 1)
		chance = 10;

	else
		chance = 0;

	return chance;
}

int lowerChance(Tile m[MAPSIZE][MAPSIZE], int oldX, int oldY)
{
	int chance;

	if(m[oldX][oldY].h == 6)
		chance = (int) m[oldX][oldY].chance * 0.40 * MULTIPLIER;

	else if(m[oldX][oldY].h == 5)
		chance = (int) m[oldX][oldY].chance * 0.50 * MULTIPLIER;

	else if(m[oldX][oldY].h == 4)
		chance = (int) m[oldX][oldY].chance * 0.60 * MULTIPLIER;

	else if(m[oldX][oldY].h == 3)
		chance = (int) m[oldX][oldY].chance * 0.80 * MULTIPLIER;

	else if(m[oldX][oldY].h == 2)
		chance = (int) m[oldX][oldY].chance * 0.1 * MULTIPLIER;

	else if(m[oldX][oldY].h == 1)
		chance = (int) m[oldX][oldY].chance * 0.1 * MULTIPLIER;

	else
		chance = (int) m[oldX][oldY].chance * 0 * MULTIPLIER;

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
	switch(n)
	{
		case 0 :
			SetConsoleTextAttribute(hConsole, 17);
			printf("%d", n);
			break;

		case 1 :
			SetConsoleTextAttribute(hConsole, 51);
			printf("%d", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 102);
			printf("%d", n);
			break;
		
		case 3 :
			SetConsoleTextAttribute(hConsole, 34);
			printf("%d", n);
			break;
		
		case 4 :
			SetConsoleTextAttribute(hConsole, 136);
			printf("%d", n);
			break;
		
		case 5 :
			SetConsoleTextAttribute(hConsole, 119);
			printf("%d", n);
			break;
		
		case 6 :
			SetConsoleTextAttribute(hConsole, 255);
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

void inicializa_fila(Fila *fila)
{
	fila->head = NULL;
	fila->tail = NULL;
}

void insere_fila(Fila *fila, Pos p)
{
	No *novo;

	novo = (No*) malloc(sizeof(No));

	novo->pos = p;
	novo->prox = NULL;

	if(fila_vazia(fila))
		fila->head = novo;

	else
		fila->tail->prox = novo;

	fila->tail = novo;
}

Pos remove_fila(Fila *fila)
{
	Pos temp_pos;
	No *temp_no;

	temp_pos = fila->head->pos;
	temp_no = fila->head;

	fila->head = fila->head->prox;

	if(fila_vazia(fila))
		fila->tail = NULL;

	free(temp_no);

	return temp_pos;
}

int fila_vazia(Fila *fila)
{
	if(fila->head == NULL)
		return 1;

	return 0;
}

void esvazia_fila(Fila *fila)
{
	No *temp;

	while(!fila_vazia(fila))
	{
		temp = fila->head;
		fila->head = fila->head->prox;

		free(temp);
	}
}