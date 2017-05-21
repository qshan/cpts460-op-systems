/****************** kernel.c file *************************/
PROC * kfork(char * filename) {
    PROC * p = get_proc( & freeList);
    int i;
    u16 u_segment, n_segment;
    if (p == 0) {
        printf("\tno more PROC, kfork() failed\n");
        return 0;
    }
    printf("\tgot proc %d from ready queue\n", p->pid);
    p->status = READY;
    p->priority = 1; // priority = 1 for all proc except P0
    p->ppid = running->pid; // parent = running
    p->parent = running;
    strcpy(p->name, pname[p->pid]);

    //KERNEL PROC CONFIG
    //+++++++++++++++++++++++++
    for (i = 1; i < 10; i++) {
        p->kstack[SSIZE - i] = 0;
    }
    //returning address in umode
    p->kstack[SSIZE - 1] = (int) body;
    //top of umode stack
    p->ksp = & (p->kstack[SSIZE - 9]);
    //add to rdy queue
    printf("\tEnqueue pid %d to readyqueue\n", p->pid);
    enqueue( & readyQueue, p); // enter p into readyQueue by priority
    nproc++;

    //USER PROC CONFIG
    //calculate offsets for start and end of user segment
    u_segment = (p->pid -1) * SEGSIZE + 0x2000;
    n_segment = (p->pid+1) * SEGSIZE + 0x2000;
    printf("u_segment = %x, n_segment = %x\n", u_segment, n_segment);
    // u_segment = (p->pid + 1) * SEGID;
    // n_segment = (p->pid + 2) * SEGID;
    //set user serg to correct value corr pid
    p->uss = u_segment;
    //set sp to top of seg
    p->usp = n_segment - 24;

    //loading filename image
    if (filename) {
        load(filename, u_segment);
    }
    //or not
    else {
        printf("\t!! NOT LOADING IMAGE !!\n");
    }
    //config ustack
    //flag and CS for int80
    //eS and DS to usersegment
    put_word(0x0200, u_segment, n_segment - 2);
    put_word(p->uss, u_segment, n_segment - 4);
    // PC for int80 and registers to 0
    //LOOP wasn't working
    put_word(0, u_segment, n_segment - 6);
    put_word(0, u_segment, n_segment - 8);
    put_word(0, u_segment, n_segment - 10);
    put_word(0, u_segment, n_segment - 12);
    put_word(0, u_segment, n_segment - 14);
    put_word(0, u_segment, n_segment - 16);
    put_word(0, u_segment, n_segment - 18);
    put_word(0, u_segment, n_segment - 20);
    put_word(u_segment, u_segment, n_segment - 22);
    put_word(u_segment, u_segment, n_segment - 24);

    printf("\treturn pointer to p\n");
    return p; // return child PROC pointer
}

int do_tswitch() {
    printf("proc %d tswitch()\n", running->pid);
    tswitch();
    printf("proc %d resumes\n", running->pid);
}

int do_kfork(char * filename) {
    PROC * p;
    printf("proc %d kfork a child\n", running->pid);
    p = kfork(filename);
    if (p == 0)
        printf("kfork failed\n");
    else
        printf("child pid = %d\n", p->pid);
}

int do_stop() {
    printf("proc %d stop running\n", running->pid);
    running->status = STOPPED;
    tswitch();
    printf("proc %d resume from stop\n", running->pid);
}

int do_continue() {
    PROC * p;
    int pid;

    printf("enter pid to resume : ");
    pid = (getc() & 0x7F) - '0';

    if (pid < 1 || pid >= NPROC) {
        printf("invalid pid\n", pid);
        return 0;
    }
    p = & proc[pid];
    if (p->status == STOPPED) {
        p->status = READY;
        enqueue( & readyQueue, p);
        return 1;
    }
    return 0;
}

int do_sleep() {
    int event;
    printf("enter an event value to sleep on: ");
    event = geti();
    ksleep(event);
    printf("proc %d resumes after sleep\n", running->pid);
}

int do_wakeup() {
    int event;
    printf("enter an event value to wake up: ");
    event = geti();
    kwakeup(event);
}

int do_exit() {
    int exitValue;
    if (running->pid == 1 && nproc > 2) {
        printf("other procs still exist, P1 can't die yet !%c\n", 007);
        return -1;
    }
    printf("enter an exitValue (0-9) : ");
    exitValue = (getc() & 0x7F) - '0';
    printf("%d\n", exitValue);
    kexit(exitValue);
}

int do_wait() {
    int child, status;
    child = kwait( & status);
    if (child < 0) {
        printf("proc %d wait error : no child\n", running->pid);
        return -1;
    }
    printf("proc %d found a ZOMBIE child %d exitValue=%d\n",
        running->pid, child, status);
    return child;
}
int reschedule() {
    PROC * p, * tempQ = 0;

    while ((p = dequeue( & readyQueue))) {
        enqueue( & tempQ, p);
    }
    readyQueue = tempQ;

    rflag = 0;
    if (running->priority < readyQueue->priority)
        rflag = 1;
}

int chpriority(int pid, int pri) {
    PROC * p;
    int i, ok = 0, reQ = 0;

    if (pid == running->pid) {
        running->priority = pri;

        if (pri < readyQueue->priority)
            rflag = 1;
        return 1;
    }
    // not for running; for both READY and SLEEP procs
    for (i = 1; i < NPROC; i++) {
        p = & proc[i];
        if (p->pid == pid && p->status != FREE) {
            p->priority = pri;
            ok = 1;
            if (p->status == READY) // in readyQueue==> redo readyQueue
                reQ = 1;
        }
    }
    if (!ok) {
        printf("chpriority failed\n");
        return -1;
    }
    if (reQ)
        reschedule(p);
}

int do_chpriority() {
    int pid, pri;
    char s[16];
    printf("input pid ");
    pid = geti();
    printf("input new priority ");
    pri = geti();
    if (pri < 1)
        pri = 1;
    chpriority(pid, pri);
}

int body() {
    char c;
    color = 0x000B + (running->pid % 5);
    printf("proc %d resumes to body()\n", running->pid);
    while (1) {
        printf("-----------------------------------------\n");
        printList("freelist  ", freeList);
        printList("readyQueue", readyQueue);
        printList("sleepList ", sleepList);
        printf("-----------------------------------------\n");

        printf("proc %d[%d] running: parent=%d\n",
            running->pid, running->priority, running->ppid);

        printf("enter a char [s|f|t|c|z|a|p|w|q|u] : ");
        c = getc();
        printf("%c\n", c);
        switch (c) {
        case 's':
            do_tswitch();
            break;
        case 'f':
            do_kfork("/bin/u1");
            break;
        case 't':
            do_stop();
            break;
        case 'c':
            do_continue();
            break;
        case 'z':
            do_sleep();
            break;
        case 'a':
            do_wakeup();
            break;
        case 'p':
            do_chpriority();
            break;
        case 'w':
            do_wait();
            break;
        case 'q':
            do_exit();
            break;
        case 'u':
            goUmode();
            break;
        }
    }
}
