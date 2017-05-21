/*********************************************************************
*   type.h - get_proc, put_proc, enquque, dequeue, printList
*********************************************************************/
#include "type.h"
/*********************************************************************
*   globals.h - PROCs, queues, OFTs and PIPES allocated
*********************************************************************/
#include "globals.h"
/*********************************************************************
*   que.c - get_proc, put_proc, enquque, dequeue, printList
*********************************************************************/
#include "que.c"
/*********************************************************************
*   odds.c - init, scheduler, int80h, set_vector, atoi, geti
*********************************************************************/
#include "odds.c"
/*********************************************************************
*   kernel.c - kfork, do_ functions for body, reschedule, body
*********************************************************************/
#include "kernel.c"
/*********************************************************************
*   wait.c - ksleep, kwakeup, kexit, kwait, ready
*********************************************************************/
#include "wait.c"
/*********************************************************************
*   forkexec.c - copysegment, kexec, fork
*********************************************************************/
#include "forkexec.c"
/*********************************************************************
*   pipe.c - kpipe,
*********************************************************************/
#include "pipe.c"
/*********************************************************************
*   int.c - entry point from int80, getpid, ps, chname, kfork, ktswitch
*       kkwait, kkexit, getc, putc, kexec, khop
*********************************************************************/
#include "int.c"

main()
{
    printf("MTX starts in main()\n");
    printf("SEGSIZE = %u words\n", SEGSIZE);

    init();      // initialize and create P0 as running
    set_vector(80, int80h);
    kfork("/bin/u1");     // P0 kfork() P1
    while(1){
      printf("P0 running\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();         // P0 switch to run P1
   }
}
