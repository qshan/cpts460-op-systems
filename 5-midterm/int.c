
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
   int a,b,c,d,r,i;

  //  printf("USERSTACK PRINTOUT USP = %x\n", running->usp);
  //  for (i = -2; i <20; i++) {
  //    printf("get word for %x = %x\n", running->usp + (i *2), get_word(running->uss, running->usp + (i *2)));
  //    getc();
  //  }

    //==> WRITE CODE TO GET get syscall parameters a,b,c,d from ustack
    a = get_word(running->uss, running->usp + 26);
    b = get_word(running->uss, running->usp + 28);
    c = get_word(running->uss, running->usp + 30);
    d = get_word(running->uss, running->usp + 32);
    //printf("b = %x", b);
    // getc();
   switch(a){
       case 0 : r = kgetpid();        break;
       case 1 : r = kps();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkfork();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;
       case 7 : r = getc();           break;
       case 8 : r = putc(b);          break;
       case 9 : r = kexec(b);         break;
       case 10: r = khop(b);          break;
       case 11: r = kpipe(b);          break;
       case 12: r = kreadpipe(b, c, d);          break;
       case 13: r = kwritepipe(b, c ,d);          break;
       case 14: r = kclosepipe(b);          break;
       case 15: r = kpfd();          break;

       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a); getc();
   }

//==> WRITE CODE to let r be the return value to Umode AX on stack
    put_word(r, running->uss, running->usp + 16);

}

//============= WRITE C CODE FOR syscall functions ======================

int kgetpid()
{
    //WRITE YOUR C code
    return running->pid;
}


int kps()
{
    int i = 0;
    char statstr[12];
    char *stat = &statstr;
    //WRITE C code to print PROC information
    printf("%s\n", delimiter);
    printf("%s\n", delimiter);
    printf("NAME\tSTATUS\tPID\tPPID\n");
    for(i = 0; i < 9; i++){
       switch(proc[i].status){
         case(0): strcpy(stat,"FREE   ");     break;
         case(1): strcpy(stat,"READY  ");    break;
         case(2): strcpy(stat,"RUNNING");  break;
         case(3): strcpy(stat,"STOPPED");  break;
         case(4): strcpy(stat,"SLEEP  "); break;
         case(5): strcpy(stat,"ZOMBIE");   break;
         default: strcpy(stat,"na");       break;
       }
       printf("%s\t%s\t%d\t%d\n", proc[i].name, stat, proc[i].pid, proc[i].ppid);
     }

     printf("%s\n", delimiter);
    // printf("I am proc %d in UMODE: running segment=%x\n", running->pid, running->uss);
    return 0;
}

int kchname(char *name)
{
    //name <32
    char buf[32], *p = buf, b;

    if(!name)
    {
      return -1;
    }
    b = get_byte(running->uss, name);
    while(b != '\0')
    {
      *p = b;
      b = get_byte(running->uss, ++name);
      p++;
    }
    *p = 0;

    //printf("name = %s", buf);

    strcpy(proc[running->pid].name, buf);
    printf("name changed\n" );
    return 0;

}

int kkfork()
{
  return fork();
  //return child pid or -1 to Umode!!!
}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{
  int id, exitval;
  //use YOUR kwait() in LAB3;
  //return values to Umode!!!
  id = kwait(&exitval);
  printf("Kwait exitvalue = %d", exitval);
  put_word(exitval, running->uss, status);

  return id;
}

int kkexit(int value)
{
    //use your kexit() in LAB3
    //do NOT let P1 die
    if(running->pid == 1)
    {
      printf("DONT KILL P1\n");
      return -1;
    }
    return kexit(value);
}

int khop(u16 newseg)
{

  u16 oldseg = running->uss;

  //copy image
  copySegment(oldseg, newseg, SEGSIZE*8);

  //config proc for new segment
  running->uss = newseg;

  //config segment for execution
  /*************************************************************************
    usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
  ----------------------------------------------------------------------------
   |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
  ----------------------------------------------------------------------------
  ***************************************************************************/
  put_word(running->uss, running->uss, running->usp); //DS
  put_word(running->uss, running->uss, running->usp+2); //ES
  put_word(running->uss, running->uss, running->usp+20); //CS in int80

  return 0;
}
