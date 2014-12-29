#include "PosQueue.h"

#include <stdlib.h>

#include "Pos.h"

NodePosQueue::NodePosQueue(Pos p, NodePosQueue* n) : pos(p)
{
	next = n;
}

PosQueue::PosQueue()
{
	head = NULL;
	tail = NULL;
}

PosQueue::~PosQueue()
{
	clearAll();
}

void PosQueue::insert(Pos p)
{
	if(isEmpty())
	{
		head = new NodePosQueue(p, NULL);
		tail = head;
	}

	else
	{
		tail->next = new NodePosQueue(p, NULL);
		tail = tail->next;
	}
}

Pos PosQueue::remove()
{
	NodePosQueue* tempNode = head;
	Pos tempPos = head->pos;

	head = head->next;

	if(isEmpty())
		tail = NULL;

	delete tempNode;

	return tempPos;
}

bool PosQueue::isEmpty()
{
	if(head == NULL)
		return true;

	return false;
}

void PosQueue::clearAll()
{
	NodePosQueue *tempNode;

	while(!isEmpty())
	{
		tempNode = head;
		head = head->next;

		delete tempNode;
	}

	tail = NULL;
}