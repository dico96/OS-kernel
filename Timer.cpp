#include <iostream>
#include <dos.h>
#include "PCB.h"
#include "SCHEDULE.h"
#include "Lock.h"
#include "IdleThr.h"

//helpers variables
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int counter = 20;
volatile int contextSwitchRequested = 0;

extern void tick();


void  interrupt timer()	
{	
	if (!contextSwitchRequested) counter--;
	if ((counter == 0 && PCB::running->timeSlice != 0) || (contextSwitchRequested == 1))
	{
		
			//saving the context

			//asm
			{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;

			// Scheduling
			//if (PCB::running!=PCB::idlePcb && PCB::running->state == Ready)
			Scheduler::put(PCB::running);


			PCB::running = Scheduler::get();
			//if (PCB::running==0) { PCB::setIdle(); PCB::running=PCB::idlePcb; }

			//restoring the context

			tsp = PCB::running->sp;
			tss = PCB::running->ss;
			tbp = PCB::running->bp;
			
			counter = PCB::running->timeSlice;

			asm
			{
				mov sp, tsp   // restore sp
				mov ss, tss
				mov bp, tbp
			}
		}
	
	if (contextSwitchRequested == 0)
	{
		asm int 60h;
		tick();
	}
	
	contextSwitchRequested = 0;
	
}

void dispatch() { // sinhrona promena konteksta 
	LOCK 
	contextSwitchRequested = 1;
	timer();
	UNLOCK
}

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

//something for protection
int isInit = 0;

void init() 
{
	if (isInit == 0)
	{
		isInit = 1;

		asm
		{
			pushf
			cli
			push es
			push ax

			mov ax,0   //  ; inicijalizuje rutinu za tajmer
			mov es,ax

			mov ax, word ptr es : 0022h //; pamti staru rutinu
			mov word ptr oldTimerSEG, ax
			mov ax, word ptr es : 0020h
			mov word ptr oldTimerOFF, ax

			mov word ptr es : 0022h, seg timer	 //postavlja 
			mov word ptr es : 0020h, offset timer //novu rutinu

			mov ax, oldTimerSEG	 //	postavlja staru rutinu	
			mov word ptr es : 0182h, ax //; na int 60h
			mov ax, oldTimerOFF
			mov word ptr es : 0180h, ax

			pop ax
			pop es
			popf
		}
	}
}

// vraca staru prekidnu rutinu
void restore()
{
	if (isInit)
	{
		asm
		{
			pushf
			cli
			push es
			push ax

			mov ax,0
			mov es,ax


			mov ax, word ptr oldTimerSEG
			mov word ptr es : 0022h, ax
			mov ax, word ptr oldTimerOFF
			mov word ptr es : 0020h, ax

			pop ax
			pop es
			popf
		}
	}
}