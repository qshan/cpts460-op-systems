//wait.c file:

//YOUR ksleep(), kwakeup(), kwait() functions

int ksleep(int event)
{
  running->status = SLEEP;
  running->event = event;
  // enter sleepList FIFO
  enqueue(&sleepList, running);
  tswitch();
}

int ready(PROC *p)
{
  p->event = 0;
  p->status = READY;
  enqueue(&readyQueue, p);
  printf("wakeup proc %d\n", p->pid);
}

/* wake up ALL procs sleeping on event */
int kwakeup(int event)
{
  PROC *p, *q = 0;

  while(p = dequeue(&sleepList)){
    if (p->event == event){
      p->status = READY;
      enqueue(&readyQueue, p);
      printf("wakeup %d\n", p->pid);
      continue;
    }
    enqueue(&q, p);
  }
  sleepList = q;
}

int kexit(int exitValue)
{
  int i; PROC *p;
  /* send children (dead or alive) to P1's orphanage */
  for (i = 1; i< NPROC; i++){
    p = &proc[i];
    if (p->status != FREE && p->ppid == running->pid){
      p->ppid = 1;
      p->parent = &proc[1];
    }
  }
  /* record exitValue and become a ZOMBIE */
  running->exitCode = exitValue;
  running->status = ZOMBIE;

  /* wakeup parent and P1 */
  kwakeup(running->parent);
  kwakeup(&proc[1]);
  tswitch();
}

int kwait(int *status)
{
  PROC *p;
  int  i, found = 0;
  while(1){
    //found = 0;
    for (i=0; i<NPROC; i++){
      p = &proc[i];
      if (p->ppid == running->pid && p->status != FREE){
        found = 1;
        /* lay the dead child to rest */
        if (p->status == ZOMBIE){
          *status = p->exitCode;
          p->status = FREE;       /* free its PROC */
          put_proc(&freeList, p);
          nproc--;
          return(p->pid);         /* return its pid */
        }
      }
    }
    if (!found)                         /* no child */
      return(-1);
    ksleep(running);                     /* has kids still alive */
  }
}
