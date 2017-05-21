struct semaphore{
  int value;
  PROC *queue;
};

int P(struct semaphore *s)
{
  int itr = 0;
  PROC *p;

  //int_off turns off interrupts, makes sure were alone
  itr = int_off();
  //dec the value
  s->value--;
  //check the value
  if(s-> value < 0)
  {
    running->status = STOPPED;
    enqueue(&(s->queue), running);
    tswitch(); //in the if? or out of the if?
  }
  //turn on interrupts again
  int_on(itr);
}

int V(struct semaphore *s)
{
  int itr = 0;
  PROC *p;

  //int_off turns off interrupts, makes sure were alone
  itr = int_off();
  //dec the value
  s->value++;
  //check the value
  if(s->value <= 0)
  {
    p = dequeue(&(s->queue));
    p->status = READY;
    enqueue(&readyQueue, p);
  }
  //turn on interrupts again
  int_on(itr);
}


/*********************************************************************
*   ------------------------------------------------------
 P(struct sempahore *s)    |  V(struct semaphore *s)
{                          |  {
  enterCR(&s->lock);       |     enterCR(&s->lock);
  s->value--;              |     s->value++;
  if (s->value < 0)        |     if (s->value <= 0)
    BLOCK(s);              |       SIGNAL(s);
  else exitCR(&s->lock);   |   exitCR(&s->lock);
}                          |   }
----------------------------------------------------------
*********************************************************************/
