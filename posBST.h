#ifndef POSBST
#define POSBST

#include "Pos.h"

class NodePosBST
{
	private:

	public:
		Pos pos; /* chave */
		NodePosBST *left, *right;

		NodePosBST(Pos p, NodePosBST *e, NodePosBST *d);
};

class PosBST
{
	private:
		NodePosBST *head;

		bool searchInside(NodePosBST *insideNode, Pos posBusca);
		bool insertInside(NodePosBST *&insideNode, Pos posInsert);

	public:

		PosBST();

		/* Retorna 1 se a chave for encontrada */
		bool search(Pos posBusca);

		/* Retorna 0 se a chave for repetida */
		bool insert(Pos posInsert);

		/* Retorna 0 se a chave não for encontrada */
		bool remove(Pos posRemove);

		Pos removeLowest();

		Pos removeHead();

		bool empty();
};

#endif