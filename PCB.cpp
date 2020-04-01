#include <stdlib.h>
#include <dos.h>
#include "PCB.h"
#include "PCBList.h"
#include "Lock.h"
#include "SCHEDULE.h"
#include "IdleThr.h"


ID PCB::lastId = 0;
PCBList* PCB::allThreadList = new PCBList();   
PCB* PCB::idlePcb = 0;
PCB* PCB::running = 0;




ID PCB::getId() const
{
	return id;
}

ID PCB::getRunningId()
{
	if (PCB::running != 0) return PCB::running->id;
	return -1;
}



Thread* PCB:: getThreadById(ID id)
{
	PCB* pcb = PCB::allThreadList->getPcbById(id);
	if (pcb != 0) return pcb->myThread;
	return 0;
}


PCB::PCB(Thread* _myThread, StackSize _stackSize, Time _timeSlice):
	//initialization list
	myThread(_myThread), stackSize(_stackSize), timeSlice(_timeSlice),
	id(++lastId), stack(0), ss(0), sp(0), bp(0), blockedWaitingList(0), isStarted(0)
{
	allThreadList->add(this);
	blockedWaitingList = new PCBList(); 
	state = New;
}

PCB::~PCB()
{
	if(stack!=0) delete[]stack;
	if(blockedWaitingList!=0) delete blockedWaitingList;
}


void PCB::createStack()
{
	if (stackSize < 0) stackSize = 256UL; //upitno zbog main stack-a
	if (stackSize > 65535UL) stackSize = 65535UL;

	StackSize topofStack = stackSize, sizeOfData;

	sizeOfData = sizeof(myThread);
	topofStack -= sizeOfData;
	*((Thread**)&stack[topofStack]) = myThread;


	topofStack -= 4;

	sizeOfData = sizeof(unsigned int);
	topofStack -= sizeOfData;
	*((unsigned int*)&stack[topofStack]) = 0x200;


	topofStack -= sizeOfData;

	*((unsigned int*)&stack[topofStack]) = FP_SEG(&wrapper);


	topofStack -= sizeOfData;

	*((unsigned int*)&stack[topofStack]) = FP_OFF(&wrapper);

	topofStack -= 9 * sizeOfData;


	ss = FP_SEG(&stack[topofStack]);
	sp = FP_OFF(&stack[topofStack]);
	bp = sp;

}
	
void PCB::wrapper(Thread* thread) //i have previously put *thread*on the stack of PCB in whose context this executes
{
	thread->run();

	//releasing the resources of the running thread(PCB) which has finished its run() method
	LOCK
	running->releaseResources();
	UNLOCK

	dispatch();
}

void PCB::releaseResources()
{	
	running->state = Finished;
	//threads which were waiting for current thread to finish should be put in sceduler as ready
	PCBList::Elem* curr = running->blockedWaitingList->first, *prev = 0;
	while (curr)
	{
		curr->pcb->state = Ready;
		Scheduler::put(curr->pcb);
		running->blockedWaitingList->delPcb(curr->pcb);
		curr = curr->next;
	}
}


void PCB::setIdle()
{
	if (idlePcb == 0)
	{
		static IdleThread* idle = new IdleThread();
		idlePcb = idle->myPCB;
		idlePcb->state = Idle;
	}
}