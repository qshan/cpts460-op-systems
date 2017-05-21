//copy entire segment from in to out
int copySegment(u16 inseg, u16 outseg, u16 size) {
    int i = 0;
    u16 word;
    printf("in copysegment for %u bytes\n", size);
                      //THIS IS THE MAGIC NUMBER = MUST BE SEGSIZE TOTAL
    for (i = 0; i < size; i++) {
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
        printf("kfork() on behalf of fork() failed!\n");
        return -1;
    }

    //COPY SEG
    printf("copying segment %x to segment %x",running->uss, newproc->uss);
    //printf("SEGSIZE = %u", SEGSIZE*8);
    copySegment(running->uss, newproc->uss, SEGSIZE*8);
    //MOD SEG
    //copy parent's SP, ss taken care of in kfork()``
    newproc->usp = running->usp;
    //copy parent's fd[] to newproc
    //printf("copy parent's fd[]\n");
    for (i = 0; i < NFD; i++) {
      if(running->fd[i])
      {
        //printf("found fd[%d]!\n", i);
        newproc->fd[i] = running->fd[i];
        newproc->fd[i]->refCount++;
        if(running->fd[i]->mode == READ_PIPE) { running->fd[i]->pipe_ptr->nreader++; }
        if(running->fd[i]->mode == WRITE_PIPE) { running->fd[i]->pipe_ptr->nwriter++; }
        //printf("refCount = %d, readers = %d, writers = %d\n", newproc->fd[i]->refCount, running->fd[i]->pipe_ptr->nreader, running->fd[i]->pipe_ptr->nwriter);
      }
    }


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
