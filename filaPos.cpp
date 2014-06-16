#include <stdlib.h>
#include "filaPos.h"


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