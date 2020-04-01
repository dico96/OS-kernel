#ifndef _PCB_h_
#define _PCB_h_


#include "Thread.h"

class PCBList;
class IdleThread;

class PCB {
public:
	//stack and registers
	StackSize stackSize; //stack size is in unsigned ; sizeof(unsigned)=2bytes
	unsigned char * stack;
	unsigned  ss, sp, bp;
	
	//time-quant given for execution before preemption happens 
	Time timeSlice;

	//Thread that I represent
	Thread* myThread;

	//thread ID
	static ID lastId;
	ID id;
	
	//possible states of a Thread
	enum State {New,Ready, Blocked, Idle, Finished};
	State state;
	
	//something for protection
	int isStarted;

	//list of all PCB's in the system
	static PCBList* allThreadList;
	
	//List of Threads(PCB's) who are waiting for me to finish
	PCBList* blockedWaitingList;

	static PCB* running;

	static PCB* idlePcb; 
	static void setIdle();

	//* METHODS *

	PCB(Thread*, StackSize, Time);
	
	void createStack();
	
	static void wrapper(Thread*);
	
	void releaseResources();
	

	~PCB(); 
	
	ID getId()const;
	static ID getRunningId();
	static Thread* getThreadById(ID id);

	friend void timer();

};




#endif
