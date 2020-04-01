#ifndef _IdleThr_h_
#define _IdleThr_h_

class IdleThread :public Thread {
public:

void run() 
{
	while (1)
	{
		dispatch();
	}
}

};

#endif