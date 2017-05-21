
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
   int a,b,c,d,r;

    //==> WRITE CODE TO GET get syscall parameters a,b,c,d from ustack
    a = get_word(running->uss, running->usp + 26);
    b = get_word(running->uss, running->usp + 28);
    c = get_word(running->uss, running->usp + 30);
    d = get_word(running->uss, running->usp + 32);
   switch(a){
       case 0 : r = kgetpid();        break;
       case 1 : r = kps();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkfork();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;

       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a);
   }

//==> WRITE CODE to let r be the return value to Umode
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
    printf("NAME\t\tSTATUS\t\tPID\t\tPPID\n");
    for(i = 0; i < 9; i++){
       switch(proc[i].status){
         case(0): strcpy(stat,"FREE");     break;
         case(1): strcpy(stat,"READY");    break;
         case(2): strcpy(stat,"RUNNING");  break;
         case(3): strcpy(stat,"STOPPED");  break;
         case(4): strcpy(stat,"SLEEP"); break;
         case(5): strcpy(stat,"ZOMBIE");   break;
         default: strcpy(stat,"na");       break;
       }
       printf("%s\t\t%s\t\t%d\t\t%d\n", proc[i].name, stat, proc[i].pid, proc[i].ppid);
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
  return kfork("/bin/u1");
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
  printf("Kwait exitvalue = %d", exitval);
  id = kwait(&exitval);

  put_word(exitval, running->uss, status);
  if(id == -1)
  {
    return -1;
  }

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
