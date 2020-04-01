#include "Thread.h"
#include "PCB.h"
#include "Lock.h"
#include "SCHEDULE.h"
#include "PCBList.h"

Thread::Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice)
{
	//LOCK
	myPCB = new PCB(this, stackSize, timeSlice);
	PCB::allThreadList->add(myPCB);
	//UNLOCK
}


void Thread::start()
{
	//LOCK
	if (myPCB->isStarted == 0)
	{
		myPCB->createStack();
		myPCB->state = PCB::Ready;
		Scheduler::put(myPCB);
		myPCB->isStarted = 1;
	}//UNLOCK
}



Thread::~Thread()
{
	//LOCK
	waitToComplete();
	PCB::allThreadList->delPcb(myPCB);
	delete myPCB;
	//UNLOCK
}


void Thread::waitToComplete()
{
	if (myPCB != 0 && myPCB != PCB::running && myPCB->state != PCB::Finished && myPCB->isStarted!=0 && myPCB!=PCB::idlePcb)
	{ 
		PCB::running->state = PCB::Blocked;
		myPCB->blockedWaitingList->add(PCB::running);
		dispatch();
	}
}

ID Thread::getId()
{
	return myPCB->getId();
}

 ID Thread:: getRunningId()
{
	//LOCK
	return PCB::getRunningId();
	//UNLOCK
}

Thread* Thread::getThreadById(ID id)
{
	return PCB::getThreadById(id);
}