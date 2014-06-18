#include <stdlib.h>
#include <stdio.h>
#include "abb.h"

NoABB::NoABB(Pos p, NoABB *e, NoABB *d)
{
	pos = p;
	esq = e;
	dir = d;
}

ABB::ABB()
{
	head = NULL;
}

/* Retorna 1 se a chave for encontrada */
bool ABB::busca(Pos posBusca)
{
	if(head == NULL)
		return false;

	else if(posBusca.getX() < head->pos.getX())
		return buscaInterna(head->esq, posBusca);

	else if(posBusca.getX() > head->pos.getX())
		return buscaInterna(head->dir, posBusca);

	else
	{
		if(posBusca.getY() < head->pos.getY())
			return buscaInterna(head->esq, posBusca);

		else if(posBusca.getY() > head->pos.getY())
			return buscaInterna(head->dir, posBusca);

		else
			return true;
	}
}

bool ABB::buscaInterna(NoABB* interno, Pos posBusca)
{
	if(interno == NULL)
		return false;

	else if(posBusca.getX() < interno->pos.getX())
		return buscaInterna(interno->esq, posBusca);

	else if(posBusca.getX() > interno->pos.getX())
		return buscaInterna(interno->dir, posBusca);

	else
	{
		if(posBusca.getY() < interno->pos.getY())
			return buscaInterna(interno->esq, posBusca);

		else if(posBusca.getY() > interno->pos.getY())
			return buscaInterna(interno->dir, posBusca);

		else
			return true;
	}
}

//*head :> *no :> no
/* Retorna 0 se a chave for repetida */
bool ABB::insere(Pos posInsert)
{
	if(head == NULL)
	{
		head = new NoABB(posInsert, NULL, NULL);
		return true;
	}

	if(posInsert.getX() < head->pos.getX())
		return insereInterno(head->esq, posInsert);

	else if(posInsert.getX() > head->pos.getX())
		return insereInterno(head->dir, posInsert);

	else
	{
		if(posInsert.getY() < head->pos.getY())
			return insereInterno(head->esq, posInsert);

		else if(posInsert.getY() > head->pos.getY())
			return insereInterno(head->dir, posInsert);

		else
			return false;
	}
}

bool ABB::insereInterno(NoABB *&interno, Pos posInsert)
{
	if(interno == NULL)
	{
		interno = new NoABB(posInsert, NULL, NULL);
		return true;
	}

	if(posInsert.getX() < interno->pos.getX())
		return insereInterno(interno->esq, posInsert);

	else if(posInsert.getX() > interno->pos.getX())
		return insereInterno(interno->dir, posInsert);

	else
	{
		if(posInsert.getY() < interno->pos.getY())
			return insereInterno(interno->esq, posInsert);

		else if(posInsert.getY() > interno->pos.getY())
			return insereInterno(interno->dir, posInsert);

		else
			return false;
	}
}

//*head :> *no :> no
/* Retorna 0 se a chave nao for encontrada */
bool ABB::remove(Pos posRemove)
{
	NoABB *aux = head, *aux2 = head, *pai = NULL, *pai2 = NULL;

	while(aux != NULL && (aux->pos.getX() != posRemove.getX() || aux->pos.getY() != posRemove.getY()))
	{
		if(posRemove.getX() < aux->pos.getX())
		{
			pai = aux;
			aux = aux->esq;
		}

		else if(posRemove.getX() > aux->pos.getX())
		{
			pai = aux;
			aux = aux->dir;
		}

		else
		{
			if(posRemove.getY() < aux->pos.getY())
			{
				pai = aux;
				aux = aux->esq;
			}

			else if(posRemove.getY() > aux->pos.getY())
			{
				pai = aux;
				aux = aux->dir;
			}
		}
	}

	if(aux == NULL)
		return false;

	else
	{
		if(aux->esq == NULL && aux->dir == NULL)
		{
			if(pai == NULL)
				head = NULL;

			else
			{
				if(pai->dir == aux)
					pai->dir = NULL;

				else
					pai->esq = NULL;
			}

			delete aux;
		}

		else if(aux->esq != NULL && aux->dir == NULL)
		{
			if(pai == NULL)
				head = aux->esq;

			else
			{
				if(pai->dir == aux)
					pai->dir = aux->esq;

				else
					pai->esq = aux->esq;
			}

			delete aux;
		}

		else if(aux->esq == NULL && aux->dir != NULL)
		{
			if(pai == NULL)
				head = aux->dir;

			else
			{
				if(pai->dir == aux)
					pai->dir = aux->dir;

				else
					pai->esq = aux->dir;
			}

			delete aux;
		}

		else
		{
			aux2 = aux->dir;
			pai2 = aux;

			while(aux2->esq != NULL)
			{
				pai2 = aux2;
				aux2 = aux2->esq;
			}

			pai2->esq = aux2->dir;

			aux2->esq = aux->esq;
			aux2->dir = aux->dir;

			if(pai == NULL)
				head = aux2;

			else
			{
				if(pai->dir == aux)
					pai->dir = aux2;

				else
					pai->esq = aux2;

				if(pai2->dir == aux)
					pai2->dir = NULL;

				else
					pai2->esq = NULL;
			}

			delete aux;
		}

		return true;
	}
}

Pos ABB::removeLowest()
{
	NoABB *temp = head;
	Pos remPos;

	while(temp->esq != NULL)
	{
		temp = temp->esq;
	}

	remPos = temp->pos;

	remove(remPos);

	return remPos;
}

bool ABB::arvoreVazia()
{
	if(head == NULL)
		return true;

	else
		return false;
}

void ABB::inorder()
{
	if (head != NULL)
	{
		inorder(head->esq);
		printf("(%d %d) ", head->pos.getX(), head->pos.getY());    
		inorder(head->dir);    
	}
}

void ABB::inorder(NoABB* a)
{
	if (a != NULL)
	{
		inorder(a->esq);
		printf("(%d %d) ", a->pos.getX(), a->pos.getY());    
		inorder(a->dir);    
	}
}