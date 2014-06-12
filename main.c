#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define SIZE 79

#define MAX_H 7

#define BRANCO 0
#define CINZA 1
#define PRETO 2

#define INFINITO 1100
#define NULO -1

//struct de posição
typedef struct pos
{
	int x;
	int y;
} Pos;

//struct de cada tile do mapa
typedef struct tile
{
	int h;
	int chance;
	int cor;
	int dist;
	Pos pred;
} Tile;

//nó da fila de posições a serem alteradas
typedef struct no
{
	Pos pos;
	struct no *prox;
} No;

typedef struct fila
{
	No *head;
	No *tail;
} Fila;

typedef No* Lista;

HANDLE hConsole;

//imprimir tile na cor certa
void printColor(int n);

//funções de fila
void inicializa_fila(Fila *fila);
void insere_fila(Fila *fila, Pos p);
Pos remove_fila(Fila *fila);
int fila_vazia(Fila *fila);
void esvazia_fila(Fila *fila);

int main()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	Tile m[SIZE][SIZE];
	Pos temp, atual;
	Fila fila;
	No *temp_no;
	Pos vetRdmPred[8];

	int vetRdmPredTam;

	int rdmChance;
	int max;

	//iterações de inserção de artefatos
	int iterations = 0, numIts;

	//contadores
	int i, j, k, l;

	inicializa_fila(&fila);
	srand(time(NULL));

	//zera todos os tiles
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
				m[i][j].h = 0;
				m[i][j].chance = 0;
				m[i][j].cor = BRANCO;
				m[i][j].dist = INFINITO;
				m[i][j].pred.x = NULO;
				m[i][j].pred.y = NULO;
		}
	}

	scanf("%d", &numIts);

while(iterations < numIts)
{
	//reseta marcadores de visitados a cada iteração
	for(i = 0; i < SIZE; i++)
		for(j = 0; j < SIZE; j++)
				m[i][j].cor = BRANCO;

	//aleatoriamente seleciona um tile para ser topo de uma montanha
	temp.x = rand() % (SIZE - 1);
	temp.y = rand() % (SIZE - 1);

	m[temp.x][temp.y].h = MAX_H - 1;
	m[temp.x][temp.y].chance = 100;
	m[temp.x][temp.y].cor = CINZA;
	m[temp.x][temp.y].dist = 0;
	m[temp.x][temp.y].pred.x = NULO;
	m[temp.x][temp.y].pred.y = NULO;

													printf("%d %d\n", temp.x, temp.y);

	insere_fila(&fila, temp);

	//checa toda a fila
	while(!fila_vazia(&fila))
	{
		//printf("iterations %d\n", iterations);

		//posição da fila a ser trabalhada
		atual = fila.head->pos;
	
		remove_fila(&fila);

		//visita posições adjacentes
		for(i = -1; i <= 1; i++)
			for(j = -1; j <= 1; j++)
			{
				//temp é a posição relativa nesta iteração a posição da fila
				temp.x = atual.x - j;
				temp.y = atual.y - i;

				
				if( (temp.x >= 0 && temp.y >= 0 && temp.x < SIZE && temp.y < SIZE)			 // posição relativa está dentro do mapa
					 //&& ((j == 0 && (i == -1 || i == 1)) || (i == 0 && (j == -1 || j == 1))) // e só varia em x OU em y (não é diagonal)
					 && !(i == 0 && j == 0)
					 && m[temp.x][temp.y].cor == BRANCO)									 // e ainda não foi visitada
				{
					m[temp.x][temp.y].cor = CINZA;							// marca como visitado
					m[temp.x][temp.y].dist = m[atual.x][atual.y].dist + 1;	// marca distância em relação a tile atual
					m[temp.x][temp.y].pred = atual;							// marca predecessor como tile atual

					insere_fila(&fila, temp);
				}
			}

		m[atual.x][atual.y].cor = PRETO;

		vetRdmPredTam = 0;

		for(i = -1; i <= 1; i++)
			for(j = -1; j <= 1; j++)
			{
				temp.x = atual.x - j;
				temp.y = atual.y - i;

				if( (temp.x >= 0 && temp.y >= 0 && temp.x < SIZE && temp.y < SIZE)
					&& m[temp.x][temp.y].cor == PRETO)
				{
					vetRdmPred[vetRdmPredTam] = temp;
					vetRdmPredTam++;
				}
			}

		temp = vetRdmPred[rand() % vetRdmPredTam];


/*
		do
		{
			temp.x = atual.x + (rand() % 3 - 1);
			temp.y = atual.y + (rand() % 3 - 1);
		} while(temp.x < 0 || temp.y < 0 || temp.x > SIZE || temp.y > SIZE || m[temp.x][temp.y].cor != PRETO);
*/

/*		if(m[atual.x][atual.y].pred.x != NULO)
			temp = m[atual.x][atual.y].pred;

		else
			temp = atual;
*/

		if(m[temp.x][temp.y].h == 6)
			m[atual.x][atual.y].chance = 60;

		else if(m[temp.x][temp.y].h == 5)
			m[atual.x][atual.y].chance = 70;

		else if(m[temp.x][temp.y].h == 4)
			m[atual.x][atual.y].chance = 80;

		else if(m[temp.x][temp.y].h == 3)
			m[atual.x][atual.y].chance = 90;	

		else if(m[temp.x][temp.y].h == 2)
			m[atual.x][atual.y].chance = 0;

		else if(m[temp.x][temp.y].h == 1)
			m[atual.x][atual.y].chance = 0;

		else
			m[atual.x][atual.y].chance = 100;


		rdmChance = rand() % 100;	// aleatoriedade para mudar ou não a altura em relação ao tile da fila

		// diminui altura baseado na chance
		if(rdmChance > m[atual.x][atual.y].chance)
		{
			if(m[temp.x][temp.y].h - 1 < 0) // se diminuição de altura for menor do que 0, é 0
				m[atual.x][atual.y].h = 0;

			else
				m[atual.x][atual.y].h = m[temp.x][temp.y].h - 1;
		}

		else
			m[atual.x][atual.y].h = m[temp.x][temp.y].h;
	}

/*
	for(j = 0; j < SIZE; j++)
	{
		for(i = 0; i < SIZE; i++)
			printColor(m[i][j].h);

		printf("\n");
	}
*/
	iterations++;
}


//*
	for(j = 0; j < SIZE; j++)
	{
		for(i = 0; i < SIZE; i++)
			printColor(m[i][j].h);

		printf("\n");
	}
//*/

	SetConsoleTextAttribute(hConsole, 7);

	return 0;
}


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
			SetConsoleTextAttribute(hConsole, 12);
			printf("%d", n);
			break;
		default:
			SetConsoleTextAttribute(hConsole, 7);		
			printf("%d", n);
			break;
	}
}