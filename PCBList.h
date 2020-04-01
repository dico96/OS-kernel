#ifndef _PCBList_h_
#define _PCBList_h_




typedef int ID;

class PCB;

class PCBList {
public:
	struct Elem {

		PCB* pcb;
		Elem* next;
		Elem(PCB* _pcb)
		{
			pcb = _pcb;
			next = 0;
		}
	};

	Elem* first, *last;

	int numOfElem;
	
	PCBList();
	~PCBList();

	void add(PCB*);
	PCB* getFirstPcb();
	PCB* getPcbById(ID id)const;
	void delPcb(PCB*);

	int size()const;

	void print()const;
};

#endif