#ifndef ABBPOS
#define ABBPOS

#include "map.h"

class NoABB
{
	private:

	public:
		Pos pos; /* chave */
		NoABB *esq, *dir;

		NoABB(Pos p, NoABB *e, NoABB *d);
};

class ABB
{
	private:
		NoABB *head;

		bool buscaInterna(NoABB *interno, Pos posBusca);
		bool insereInterno(NoABB *&interno, Pos posInsert);

	public:

		ABB();

		/* Retorna 1 se a chave for encontrada */
		bool busca(Pos posBusca);

		/* Retorna 0 se a chave for repetida */
		bool insere(Pos posInsert);

		/* Retorna 0 se a chave não for encontrada */
		bool remove(Pos posRemove);

		Pos removeLowest();

		bool arvoreVazia();


		void inorder();
		void inorder(NoABB* a);
};

#endif