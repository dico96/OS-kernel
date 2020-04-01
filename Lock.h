#ifndef _Lock_h_
#define _Lock_h_

#define LOCK asm { pushf; cli; } 

#define UNLOCK asm popf;


#endif

