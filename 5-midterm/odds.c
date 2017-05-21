int init()
{
    PROC *p; int i, j;
    color = 0x0C;
    printf("init ....");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;
        strcpy(proc[i].name, pname[i]);
        //0 out fd array
        for (j = 0; j < NFD; j++) {
          p->fd[j] = 0;
        }
        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;
    //OFT init
    for ( i = 0; i < NOFT; i++) {
      oft[i].mode = READ_PIPE;
      oft[i].refCount = 0;
      oft[i].pipe_ptr = 0;
    }
    //PIPE init
    for (i = 0; i < NPIPES; i++) {
      memset(pipes[i].buf, 0, PSIZE);
      pipes[i].head = pipes[i].tail = 0;
      pipes[i].data = 0;
      pipes[i].room = PSIZE;
      pipes[i].nreader = pipes[i].nwriter = 0;
    }
    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
}

int scheduler()
{
    if (running->status == RUNNING)
    {
      running->status = READY;
      enqueue(&readyQueue, running);
    }
     running = dequeue(&readyQueue);
     running->status = RUNNING;
     color = running->pid + 0x0A;
}

int int80h();
int set_vector(u16 vector, u16 handler)
{
     // put_word(word, segment, offset)
     put_word(handler, 0, vector<<2);
     put_word(0x1000,  0,(vector<<2) + 2);
}

int atoi(char * s) {
    int v = 0;
    while ( * s) {
        v = v * 10 + ( * s - '0');
        s++;
    }
    return v;
}

int geti() {
    char s[16];
    gets(s);
    return atoi(s);
}
