tswitch and
======================

## Lab Assignment 2

```
Chapter 4
```

This lab was to demonstrate switching processes from within the kernel. tswitch() is an assembly function that in essence dumps the state of a process (the register values and such) to the stack, then proceeds to load a waiting process and it's state from that processes stack then resume executing.
In this lab we had to develop a priority queue to hold process structs as well as the code to resume from switching processes.

## Priority Queue

Since each struct has a priority, we will use that to create a queue where the highest priority items are at the head of the queue. then when it comes time to switch processes the highest priority task will be popped from the head of the queue and set as the running process.

The queue is defined in que.c. the enqueue() function is where we handle inserting structs by priority. First we verify that our queue is not empty and that the struct we are queueing is not the "highest priority"

``` C
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

```
If we don't belong at the head of the list then we traverse until we find our insertion point. Note the <= as we compare priorities, this is to ensure if we have 2 or more processes of equal priority the process that has been waiting the longest gets run first.

``` C
int enqueue(PROC **queue, PROC *p)
{

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

```


## tswitch()
When we switch processes we save all of the registers in our CPU to the stack, since each process has it's own stack space in memory we can save the state of our process mid-execution. tswitch() is the function that does this:

``` asm
_tswitch:
SAVE:
	push ax                    !save all registers to stack
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	pushf                      !push flags (2 bytes)

  !ds = ss = 0x1000
  push ds
  push ss

  ! set the stack pointer in _running struct
	mov  bx,_running
	mov  2[bx],sp
```
We push all of the registers and then we save the process and the stack pointer for that process onto the stack as well. Then we call the scheduler function in t.c. scheduler() enqueues the running proc struct to the priority queue of ready processes and dequeues the next proc struct in the queue.


``` asm

FIND:	call _scheduler   !SWITCH
```
Then we go to resume() which does essentially the reverse of tswitch() and sets all of the register values to the corresponding values on the stack.

``` asm
_resume:
RESUME:
	mov  bx,_running         ! re-establish stack pointer from *ksp (_running)
	mov  sp,2[bx]

  !restoring these to ss/ds cause the program to begin again at main
  pop ss
  !mov ss,ax
  pop ds
  !mov ds,ax
	popf                     !pop flags
	pop  di                  ! load all registers from stack (rev order)
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax

	ret
```
_Note: When I instantiate proc structs in kfork() they all DS/SS are set to 0x1000 rather than a distinct memory space for each proc. This is remedied in later labs_
