#ifndef POSQUEUE
#define POSQUEUE

#include "map.h"

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

		void insert(Pos p);
		Pos remove();
		bool empty();
		void clearAll();
};

#endif