//return a FREE PROC pointer from list
PROC *get_proc(PROC **list)
{
  PROC *ret = *list;
  //guard for null
  if(ret)
  {
    *list = (*list)->next;
  }
  //get first element
  //ret = *list;
  //null next
  ret->next = 0;
  //move the list forward
  //return a proc*
  return ret;
}

//enter p into list
int put_proc(PROC **list, PROC *p)
{
  if(!p){
    return 0;
  }
  //append p to front of list
  p->next = *list;
  *list = p;
}

// //enter p into queue by priority
int enqueue(PROC **queue, PROC *p)
{
  //grap front of queue;
  PROC *qp = *queue;

  //empty queue
  if (!qp ) {
    p->next = qp;
    *queue = p;
    return 1;
  }
  //else we should be at front
  if(p->priority > qp->priority)
  {
    //set to top
    p->next = qp;
    *queue = p;
    return 1;
  }

  //else at least one node to find place
  //check p->priority against the NEXT items->priority to make insert easier
  while(qp->next && p->priority <= (qp->next)->priority)
  {
    //move the thing to the next thing ;)
    qp = qp->next;
  }
  //insert p into list after qp
  p->next = qp->next;
  qp->next = p;
  return 1;
}



//return first element removed from queue
PROC *dequeue(PROC **queue)
{
  //grab "front" of queue
  PROC *pp = *queue;
  //empty queue?
  if(!pp){
    return 0;
  }
  //move queue to next item
  *queue = (*queue)->next;
  return pp;
}


int printList(char *name, PROC *p)
{
  //name of list
  printf("%s\t = ", name);
  while(p)
  {
    printf("%d -> ", p->pid);
    p = p->next;
  }
  printf("END\n");
}
