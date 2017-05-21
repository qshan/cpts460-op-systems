int goUmode();
int int80h();
int tinth();
int ireturn();

int init()
{
    PROC *p; int i;
    color = 0x0C;
    printf("init ....\n");

    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;
        strcpy(proc[i].name, pname[i]);
        p->inkmode = 1;
        p->time = SWITCH_TIME;
        p->next = &proc[i+1];
    }

    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

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


int set_vector(vector, addr)
{
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
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
