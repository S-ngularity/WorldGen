#ifndef POSQUEUE
#define POSQUEUE

#include "Pos.h"

class NodePosQueue
{
	private:

	public:
		Pos pos;
		NodePosQueue *next;

		NodePosQueue(Pos p, NodePosQueue* n);
};

class PosQueue
{
	private:
		NodePosQueue *head;
		NodePosQueue *tail;

	public:
		PosQueue();
		~PosQueue();

		void insert(Pos p);
		Pos remove();
		bool isEmpty();
		void clearAll();
};

#endif