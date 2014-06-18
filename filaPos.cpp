#include <stdlib.h>
#include "filaPos.h"

NoFila::NoFila(Pos p, NoFila* n)
{
	pos = p;
	next = n;
}

Fila::Fila()
{
	head = NULL;
	tail = NULL;
}

void Fila::insere_fila(Pos p)
{
	if(fila_vazia())
	{
		head = new NoFila(p, NULL);
		tail = head;
	}

	else
	{
		(*tail).next = new NoFila(p, NULL);
		tail = (*tail).next;
	}
}

Pos Fila::remove_fila()
{
	NoFila* tempNoFila = head;
	Pos tempPos = (*head).pos;

	head = (*head).next;

	if(fila_vazia())
		tail = NULL;

	delete tempNoFila;

	return tempPos;
}

bool Fila::fila_vazia()
{
	if(head == NULL)
		return true;

	return false;
}

void Fila::esvazia_fila()
{
	NoFila *tempNoFila;

	while(!fila_vazia())
	{
		tempNoFila = head;
		head = (*head).next;

		delete tempNoFila;
	}
}