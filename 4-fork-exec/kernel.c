//kernel.c file:

//YOUR kernel.c file as in LAB3

/****************** kernel.c file *************************/
int rflag, body();

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
    u_segment = (p->pid + 1) * 0x1000;
    n_segment = (p->pid + 2) * 0x1000;
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
        printf("!! NOT LOADING IMAGE !!\n");
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

//copy entire segment from in to out
int copySegment(int inseg, int outseg) {
    u16 i = 0, word;
    printf("in copysegment\n");
                      //THIS IS THE MAGIC NUMBER = MUST BE 64KB TOTAL
    for (i = 0; i < 32*1024; i++) {
        word = get_word(inseg, i*2);
        put_word(word, outseg, i*2);
    }
    printf("done copying exiting copysegment\n");

}


int fork() {
    int i;
    PROC * newproc;

    //create a child proc ready to run in Kmode, retrun -1 if fails;
    newproc = kfork(0);
    //no procs left
    if (newproc == 0) {
        printf("kfork() on bealf of fork() failed!\n");
        return -1;
    }

    //COPY SEG
    printf("copying segment %x to segment %x",running->uss, newproc->uss);
    // /copyImage(running->uss, newproc->uss, 32*1024);
    copySegment(running->uss, newproc->uss);
    //MOD SEG
    //copy parent's SP, ss taken care of in kfork()
    newproc->usp = running->usp;
    //fix DS/ES/AX/CS in new seg
    put_word(newproc->uss, newproc->uss, newproc->usp); //DS
    put_word(newproc->uss, newproc->uss, newproc->usp + 2); //ES
    put_word(0, newproc->uss, newproc->usp + 16); //AX
    put_word(newproc->uss, newproc->uss, newproc->usp + 20); //CS

    //RETURN
    return newproc->pid;
}






int kexec(char * command) {
    //GET STRING FROM USERSPACE
    //name <32
    char buf[32], filename[16], cmdline[32], * p, b, * c;
    int len = 0, i, stkp;

    p = cmdline;
    c = command;

    //printf("command = %x\n", c);

    if (c == 0) {
        printf("no cmd line in exec\n");
        return -1;
    }

    b = get_byte(running->uss, c);
    //printf("got char %c from %x\n", b, c);

    while (b != '\0') {
        len++; * p = b;
        b = get_byte(running->uss, ++c);
        p++;
    }
    len++; * p = 0;

    printf("original string (len %d) from umode = %s\n", len, cmdline);


    /*************************************************************************
      usp  1   2   3   4   5   6   7   8   9  10   11  12  13  14  15  16
    ----------------------------------------------------------------------------
     |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|s->| u | 2 |   | a |   | b |
    ----------------------------------------------------------------------------
    ***************************************************************************/
    //check for even str length
    if ((len) % 2 != 0) {
        printf("not even, append a 0\n");
        //pad if not even
        len += 1; * (++p) = 0;
    }


    //LOAD FILENAME TO SEGMENT
    p = cmdline;
    c = filename;
    while ( * p != ' ' && * p != 0) { * c = * p;
        p++;
        c++;
    } * c = 0;
    //buf = filename;
    //printf("filename = %s\n", filename);
    strcpy(buf, "/bin/");
    strcat(buf, filename);
    //TODO: allow full filenames?
    printf("attempting to load %s to segment %x", buf, running->uss);
    i = load(buf, running->uss);
    //printf("Load completed in kexec\n");
    if (i == 0) {
        printf("Load Failed\n");
        return -1;
    }

    //printf("config ustack init\n");
    //printf("start copy of string\n");
    //copy to new segment at high address
    for (i = 1; i < len+1; i++) {
        //printf("putting char (%c) at address %x\n", buf[len - i], (running->uss + SEGSIZE) - i);
        put_byte(cmdline[len - i], running->uss, (running->uss + SEGSIZE) - i);
    }
    //printf("putting word at address %x\n", (running->uss + SEGSIZE) - len - 2);
    put_word((running->uss + SEGSIZE) - len, running->uss, ((running->uss + SEGSIZE) - len - 2));
    stkp = ((running->uss + SEGSIZE) - len - 2);
    printf("stkpt = %x\n", stkp);
    //printf("string copied\n");

    // config stack with argc argv and VA=0
    //set sp to top of seg
    //TODO: VERIFY STKTP = 0XSFF6 AND NEW USP = 0X2FDE
    running->usp = stkp - 24;
    //config ustack
    //flag and CS for int80
    //eS and DS to usersegment
    // put_word(0x0200, running->uss, running->uss + SEGSIZE-2);
    put_word(0x0200, running->uss, stkp - 2);
    // put_word(running->uss,running->uss,  running->uss + SEGSIZE- 4);
    put_word(running->uss, running->uss, stkp - 4);

    //LOOP wasn't working
    put_word(0, running->uss, stkp - 6);
    put_word(0, running->uss, stkp - 8);
    put_word(0, running->uss, stkp - 10);
    put_word(0, running->uss, stkp - 12);
    put_word(0, running->uss, stkp - 14);
    put_word(0, running->uss, stkp - 16);
    put_word(0, running->uss, stkp - 18);
    put_word(0, running->uss, stkp - 20);
    put_word(running->uss, running->uss, stkp - 22);
    put_word(running->uss, running->uss, stkp - 24);

    printf("config ustack complete, returning\n");
}

// int kexec(char *y)
// {
//   int i, len;
//    u16 segment, offset, HIGH;
//    char line[64], *cp, c;
//
//    char filename[32], f2[16],*cq;
//
//    segment = running->uss; // if vfork()ed, we are using parent's segment
//   /* get command line line from U space */
//    cp = line;
//    while( (*cp=get_byte(segment, y)) != 0 ){
//           y++; cp++;
//    }
//
//    // now we are using our own segment
//    segment = (running->pid+1)*0x1000;
//    //   printf("exec : line=%s\n", line);
//    /* extract filename to exec */
//    cp = line;        cq = f2;
//
//    while(*cp == ' ')  /* SAFETY: skip over leading blanks */
//      cp++;
//
//    while (*cp != ' ' && *cp != 0){
//          *cq = *cp;
//           cq++; cp++;
//    }
//    *cq = 0;
//
//    if (f2[0]==0){
//        return -1;     /* should NOT happen but just in case */
//    }
//
//    //printf("exec : cmd=%s   len=%d\n", f2, strlen(f2));
//    strcpy(filename, "/bin/");   /* all executables are in /bin */
//    strcat(filename, f2);
//    //   strcpy(running->name, f2);   /* program name */
//
//    printf("Proc %d exec to %s in segment %x\n",
//            running->pid, filename, segment);
//
//    if (!load(filename, segment))
//      return -1;
//
//    len = strlen(line) + 1;
//    if (len % 2)   // odd ==> must pad a byte
//      len ++;
//
//    offset = -len;
//    // put_uword()/put_ubyte() are relative to running->uss
//    for (i=0; i<len; i++){
//        put_byte(line[i], segment, offset+i);
//    }
//
//    HIGH = offset - 2;  /* followed by INT stack frame */
//
//    /* *s pointing at the command line string */
//    put_word(offset, segment, HIGH);
//
//    /* zero out U mode registers in ustack di to ax */
//    for (i=1; i<=12; i++){
//      put_word(0, segment, HIGH-2*i);
//    }
//
//    // must set PROC.uss to our own segment
//    running->uss = segment;
//    running->usp = HIGH-2*12;
//    /* re-initialize usp to new ustack top */
//    /*   0    1   2   3  4  5  6  7  8  9  10 11 12
//    /*      flag uCS uPC ax bx cx dx bp si di es ds */
//    /* HIGH                                      sp */
//
//    put_word(running->uss, segment, HIGH-2*12);   // uDS=uSS
//    put_word(running->uss, segment, HIGH-2*11);   // uES=uSS
//    put_word(0,            segment, HIGH-2*3);    // uPC=0
//    put_word(segment,      segment, HIGH-2*2);    // uCS=segment
//    put_word(0x0200,       segment, HIGH-2*1);    // flag
// }




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
