#include "PCBList.h"
#include "PCB.h"
#include <iostream> 

using namespace std;

class PCB;

PCBList::PCBList()
{
	first = last = 0;
	numOfElem = 0;
}

PCBList::~PCBList()
{
	Elem* prev;
	while (first)
	{
		prev = first;
		first = first->next;
		delete prev;
	}
	first = last = 0;
	numOfElem = 0;
}

void PCBList::add(PCB* pcb)
{
	Elem* newElem = new Elem(pcb);
	if (first==0) first = newElem;
	else last->next = newElem;
	last = newElem;
	numOfElem++;
}

PCB* PCBList::getFirstPcb()
{
	if (first==0) return 0;
	else
	{
		Elem* temp = first;
		PCB* pcbForReturn = temp->pcb;
		delete temp;
		numOfElem--;
		first = first->next;
		if (first == 0) last = 0;
		return pcbForReturn;
	}
}


PCB* PCBList::getPcbById(ID id) const
{
	Elem* temp = first;
	while (temp && (temp->pcb->getId() != id)) temp = temp->next;
	if (temp != 0) return temp->pcb;
	return 0;
}


void PCBList::delPcb(PCB* _pcb)
{
	Elem* prev = 0, *curr = first;
	while (curr && (curr->pcb != _pcb))
	{
		prev = curr;
		curr = curr->next;
	}
	
	if (curr != 0) // we found the pcb for deleting
	{
		if (prev == 0) //the first element of the list is to be deleted
		{
			first = first->next;
			if (first == 0) last = 0;
		}
		else
		{
			prev->next = curr->next;
			if (last == curr) last = prev;
		}
		
		delete curr;
		numOfElem--;
	}
}


int PCBList::size()const
{
	return numOfElem;
}

void PCBList::print()const
{
	Elem*temp = first;
	while (temp)
	{
		cout << "PCB:" << temp->pcb->getId() << " -> ";
		temp = temp->next;
	}
}