#include <stdlib.h>
#include "PosBst.h"

NodePosBST::NodePosBST(Pos p, NodePosBST *l, NodePosBST *r) : pos(p)
{
	left = l;
	right = r;
}

PosBST::PosBST()
{
	head = NULL;
}

/* Retorna 1 se a chave for encontrada */
bool PosBST::search(Pos posSearch)
{
	if(head == NULL)
		return false;

	else if(posSearch.getX() < head->pos.getX())
		return searchInside(head->left, posSearch);

	else if(posSearch.getX() > head->pos.getX())
		return searchInside(head->right, posSearch);

	else
	{
		if(posSearch.getY() < head->pos.getY())
			return searchInside(head->left, posSearch);

		else if(posSearch.getY() > head->pos.getY())
			return searchInside(head->right, posSearch);

		else
			return true;
	}
}

bool PosBST::searchInside(NodePosBST* insideNode, Pos posSearch)
{
	if(insideNode == NULL)
		return false;

	else if(posSearch.getX() < insideNode->pos.getX())
		return searchInside(insideNode->left, posSearch);

	else if(posSearch.getX() > insideNode->pos.getX())
		return searchInside(insideNode->right, posSearch);

	else
	{
		if(posSearch.getY() < insideNode->pos.getY())
			return searchInside(insideNode->left, posSearch);

		else if(posSearch.getY() > insideNode->pos.getY())
			return searchInside(insideNode->right, posSearch);

		else
			return true;
	}
}

//*head :> *no :> no
/* Retorna 0 se a chave for repetida */
bool PosBST::insert(Pos posInsert)
{
	if(head == NULL)
	{
		head = new NodePosBST(posInsert, NULL, NULL);
		return true;
	}

	if(posInsert.getX() < head->pos.getX())
		return insertInside(head->left, posInsert);

	else if(posInsert.getX() > head->pos.getX())
		return insertInside(head->right, posInsert);

	else
	{
		if(posInsert.getY() < head->pos.getY())
			return insertInside(head->left, posInsert);

		else if(posInsert.getY() > head->pos.getY())
			return insertInside(head->right, posInsert);

		else
			return false;
	}
}

bool PosBST::insertInside(NodePosBST *&insideNode, Pos posInsert)
{
	if(insideNode == NULL)
	{
		insideNode = new NodePosBST(posInsert, NULL, NULL);
		return true;
	}

	if(posInsert.getX() < insideNode->pos.getX())
		return insertInside(insideNode->left, posInsert);

	else if(posInsert.getX() > insideNode->pos.getX())
		return insertInside(insideNode->right, posInsert);

	else
	{
		if(posInsert.getY() < insideNode->pos.getY())
			return insertInside(insideNode->left, posInsert);

		else if(posInsert.getY() > insideNode->pos.getY())
			return insertInside(insideNode->right, posInsert);

		else
			return false;
	}
}

//*head :> *no :> no
/* Retorna 0 se a chave nao for encontrada */
bool PosBST::remove(Pos posRemove)
{
	NodePosBST *aux = head, *aux2 = head, *father = NULL, *father2 = NULL;

	while(aux != NULL && (aux->pos.getX() != posRemove.getX() || aux->pos.getY() != posRemove.getY()))
	{
		if(posRemove.getX() < aux->pos.getX())
		{
			father = aux;
			aux = aux->left;
		}

		else if(posRemove.getX() > aux->pos.getX())
		{
			father = aux;
			aux = aux->right;
		}

		else
		{
			if(posRemove.getY() < aux->pos.getY())
			{
				father = aux;
				aux = aux->left;
			}

			else if(posRemove.getY() > aux->pos.getY())
			{
				father = aux;
				aux = aux->right;
			}
		}
	}

	if(aux == NULL)
		return false;

	else
	{
		if(aux->left == NULL && aux->right == NULL)
		{
			if(father == NULL)
				head = NULL;

			else
			{
				if(father->right == aux)
					father->right = NULL;

				else
					father->left = NULL;
			}

			delete aux;
		}

		else if(aux->left != NULL && aux->right == NULL)
		{
			if(father == NULL)
				head = aux->left;

			else
			{
				if(father->right == aux)
					father->right = aux->left;

				else
					father->left = aux->left;
			}

			delete aux;
		}

		else if(aux->left == NULL && aux->right != NULL)
		{
			if(father == NULL)
				head = aux->right;

			else
			{
				if(father->right == aux)
					father->right = aux->right;

				else
					father->left = aux->right;
			}

			delete aux;
		}

		else
		{
			aux2 = aux->right;
			father2 = aux;

			while(aux2->left != NULL)
			{
				father2 = aux2;
				aux2 = aux2->left;
			}

			if(father2 != aux)
			{
				father2->left = aux2->right;
				aux2->right = aux->right;
			}

			aux2->left = aux->left;

			if(father == NULL)
				head = aux2;

			else if(father->right == aux)
				father->right = aux2;

			else
				father->left = aux2;

			delete aux;
		}

		return true;
	}
}

Pos PosBST::removeLowest()
{
	NodePosBST *temp = head;
	Pos remPos;

	while(temp->left != NULL)
	{
		temp = temp->left;
	}

	remPos = temp->pos;

	remove(remPos);

	return remPos;
}

Pos PosBST::removeHead()
{
	Pos remPos = head->pos;

	remove(remPos);

	return remPos;
}

bool PosBST::isEmpty()
{
	if(head == NULL)
		return true;

	else
		return false;
}