#include <stdlib.h>
#include "posQueue.h"

NodePosQueue::NodePosQueue(Pos p, NodePosQueue* n)
{
	pos = p;
	next = n;
}

PosQueue::PosQueue()
{
	head = NULL;
	tail = NULL;
}

void PosQueue::insert(Pos p)
{
	if(empty())
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

	if(empty())
		tail = NULL;

	delete tempNode;

	return tempPos;
}

bool PosQueue::empty()
{
	if(head == NULL)
		return true;

	return false;
}

void PosQueue::clearAll()
{
	NodePosQueue *tempNode;

	while(!empty())
	{
		tempNode = head;
		head = head->next;

		delete tempNode;
	}

	tail = NULL;
}