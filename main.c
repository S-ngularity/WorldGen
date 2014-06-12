#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define MAPSIZE 25

#define MULTIPLIER 1

#define MAX_H 7

#define BRANCO 0
#define CINZA 1
#define PRETO 2

#define INFINITO 1100
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
	int cor;
	int dist;
	Pos pred;
	int isSeed;
	int printPred;
	int notLower;
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

HANDLE hConsole; // console do Windows (pra mudar cores dos tiles quando for imprimir)

//set chance de próximos tiles manterem altura
int setBaseChance(Tile m[MAPSIZE][MAPSIZE], int x, int y); // retorna chance base do tile de acordo com altura
int lowerChance(Tile m[MAPSIZE][MAPSIZE], int oldX, int oldY); // retorna nova chance diminuida baseada no tile anterior

//imprimir tile na cor certa (no Windows)
void printMap(Tile m[MAPSIZE][MAPSIZE]);
void printColor(int n);
void printSeed(int n);
void printPred(int n);

//funções de fila
void inicializa_fila(Fila *fila);
void insere_fila(Fila *fila, Pos p);
Pos remove_fila(Fila *fila);
int fila_vazia(Fila *fila);
void esvazia_fila(Fila *fila);

int main()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // seleciona o console sendo usado

	Tile m[MAPSIZE][MAPSIZE]; 	// mapa
	Pos atual, temp;			// posição atual retirada da lista e temp pra manipulações
	Fila filaAtual, filaLower;

	int max; // altura máxima dos adjacentes

	//iterações de inserção de seeds
	int iterations = 0, numIts, hAtual;
	int numMedTerrain;

	//contadores
	int i, j, k, l;

	int aux1, aux2;

	inicializa_fila(&filaAtual);
	inicializa_fila(&filaLower);
	srand(time(NULL));

	// zera todos os tiles
	for(i = 0; i < MAPSIZE; i++)
		for(j = 0; j < MAPSIZE; j++)
		{
				m[i][j].h = 0;
				m[i][j].chance = 0;
				m[i][j].cor = BRANCO;
				m[i][j].dist = INFINITO;
				m[i][j].pred.x = NULO;
				m[i][j].pred.y = NULO;
				m[i][j].isSeed = 0;
				m[i][j].printPred = 0;
				m[i][j].notLower = 0;

		}

	scanf("%d", &numMedTerrain);
	scanf("%d", &numIts);

	while(iterations < numIts)
	{
		// reseta marcadores de visitados a cada iteração
		for(i = 0; i < MAPSIZE; i++)
			for(j = 0; j < MAPSIZE; j++)
					m[i][j].cor = BRANCO;

		// aleatoriamente seleciona um tile seed
		temp.x = rand() % MAPSIZE;
		temp.y = rand() % MAPSIZE;

		if(numMedTerrain > 0)
		{
			do{
				temp.x = rand() % MAPSIZE;
				temp.y = rand() % MAPSIZE;
				aux1 = 3;
			} while(aux1 < m[temp.x][temp.y].h);

			numMedTerrain--;
		}

		else
		{
			do{
				//temp.x = rand() % MAPSIZE;
				//temp.y = rand() % MAPSIZE;
				aux1 = rand() % MAX_H;
			} while(aux1 < m[temp.x][temp.y].h);
		}

		m[temp.x][temp.y].h = aux1;
		m[temp.x][temp.y].chance = setBaseChance(m, temp.x, temp.y);
		m[temp.x][temp.y].cor = CINZA;
		m[temp.x][temp.y].dist = 0;
		m[temp.x][temp.y].pred.x = NULO;
		m[temp.x][temp.y].pred.y = NULO;
		m[temp.x][temp.y].isSeed = 1;

		insere_fila(&filaAtual, temp);

		hAtual = aux1;

		while(hAtual > 0)
		{
			// checa toda a fila
			while(!fila_vazia(&filaAtual))
			{
				atual = remove_fila(&filaAtual); // posição da fila a ser trabalhada

				// checa posições adjacentes a posição original da fila e as modifica
				for(i = -1; i <= 1; i++)
					for(j = -1; j <= 1; j++)
					{
						// temp é a posição adjacente nesta iteração
						temp.x = atual.x - j;
						temp.y = atual.y - i;

						
						if(!(i == 0 && j == 0)															// posição é adjacente
						 //&& ((j == 0 && (i == -1 || i == 1)) || (i == 0 && (j == -1 || j == 1)))		// e só varia em x OU em y (não é diagonal)
						 && (temp.x >= 0 && temp.y >= 0 && temp.x < MAPSIZE && temp.y < MAPSIZE)		// e está dentro do mapa
						 && m[temp.x][temp.y].cor == BRANCO											// e ainda não foi visitada
						 && m[atual.x][atual.y].h > m[temp.x][temp.y].h)								// e adjacente é menor que tile da fila
						{
							m[temp.x][temp.y].cor = CINZA;							// marca como visitado
							m[temp.x][temp.y].dist = m[atual.x][atual.y].dist + 1;	// marca distância em relação a tile seed
							m[temp.x][temp.y].pred = atual;							// marca predecessor como posição que foi tirada da fila

							// diminui ou não altura da adjacente em relação ao tile original
							if(rand() % 100 <= m[atual.x][atual.y].chance)	// mantem altura e diminui chance dos próximos manterem
							{ 
								m[temp.x][temp.y].h = hAtual;
								m[temp.x][temp.y].chance = lowerChance(m, atual.x, atual.y);

								m[temp.x][temp.y].notLower = 1;
//printMap(m);
								insere_fila(&filaAtual, temp);	// coloca tile de mesma altura na fila de altura atual
							}

							else  // ou diminui altura em relação ao maior em volta do temp (não necessariamente o tile original da fila)
							{
								// e reseta chance de manter os próximos
								//m[temp.x][temp.y].chance = setBaseChance(m, temp.x, temp.y);
								
								insere_fila(&filaLower, temp);	// coloca tile alterado na fila
							} // rand atual/lower
						} // hAtual > membro atual.h
					} // para todos os adjacentes do membro atual da fila

				//m[atual.x][atual.y].cor = PRETO;
			} // enquanto filaAtual não estiver vazia

			hAtual--;

			while(!fila_vazia(&filaLower))
			{
				atual = remove_fila(&filaLower);

				if(m[atual.x][atual.y].notLower == 0)
				{
					m[atual.x][atual.y].notLower = 1;
					m[atual.x][atual.y].h = hAtual;
					m[atual.x][atual.y].chance = setBaseChance(m, atual.x, atual.y);
					insere_fila(&filaAtual, atual);
//printMap(m);
				}
			}

			for(i = 0; i < MAPSIZE; i++)
				for(j = 0; j < MAPSIZE; j++)
					if(m[i][j].h < hAtual)
						m[i][j].notLower = 0;

		} // enquanto filaAtual não estiver vazia
			
		//for(i = 0; i < MAPSIZE; i++)
		//	for(j = 0; j < MAPSIZE; j++)
		//		m[i][j].notLower = 0;

		//esvazia_fila(&filaLower);

printMap(m);
		
		iterations++;
	} // para todas as iterações

	//printMap(m);
 //*
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
//*/

	return 0;
}

int setBaseChance(Tile m[MAPSIZE][MAPSIZE], int x, int y)
{
	int chance;

	if(m[x][y].h == 6)
		chance = 40;

	else if(m[x][y].h == 5)
		chance = 50;

	else if(m[x][y].h == 4)
		chance = 60;

	else if(m[x][y].h == 3)
		chance = 90;	

	else if(m[x][y].h == 2)
		chance = 0;

	else if(m[x][y].h == 1)
		chance = 0;

	else
		chance = 0;

	return chance;
}

int lowerChance(Tile m[MAPSIZE][MAPSIZE], int oldX, int oldY)
{
	int chance;

	if(m[oldX][oldY].h == 6)
		chance = (int) m[oldX][oldY].chance * 0.30 * MULTIPLIER;

	else if(m[oldX][oldY].h == 5)
		chance = (int) m[oldX][oldY].chance * 0.40 * MULTIPLIER;

	else if(m[oldX][oldY].h == 4)
		chance = (int) m[oldX][oldY].chance * 0.50 * MULTIPLIER;

	else if(m[oldX][oldY].h == 3)
		chance = (int) m[oldX][oldY].chance * 0.70 * MULTIPLIER;

	else if(m[oldX][oldY].h == 2)
		chance = (int) m[oldX][oldY].chance * 0.1 * MULTIPLIER;

	else if(m[oldX][oldY].h == 1)
		chance = (int) m[oldX][oldY].chance * 0 * MULTIPLIER;

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

				else if(m[i][j].isSeed == 1)
					printSeed(m[i][j].h);

				else
					printColor(m[i][j].h);
			}

		printf("\n");
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
			SetConsoleTextAttribute(hConsole, 176);
			printf("%d", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 240);
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
			SetConsoleTextAttribute(hConsole, 116);
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
			SetConsoleTextAttribute(hConsole, 187);
			printf("%d", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 255);//15
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