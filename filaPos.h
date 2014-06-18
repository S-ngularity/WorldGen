#ifndef FILAPOS
#define FILAPOS

#include "map.h"

class NoFila
{
	private:

	public:
		Pos pos;
		NoFila *next;

		NoFila(Pos p, NoFila* n);
};

class Fila
{
	private:
		NoFila *head;
		NoFila *tail;

	public:
		Fila();

		void inicializa_fila();
		void insere_fila(Pos p);
		Pos remove_fila();
		bool fila_vazia();
		void esvazia_fila();
};

#endif