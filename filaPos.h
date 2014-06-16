#ifndef FILAPOS
#define FILAPOS

#include "map.h"

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

//funções de fila
void inicializa_fila(Fila *fila);
void insere_fila(Fila *fila, Pos p);
Pos remove_fila(Fila *fila);
int fila_vazia(Fila *fila);
void esvazia_fila(Fila *fila);

#endif