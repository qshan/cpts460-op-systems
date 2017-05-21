#include "ucode.c"
int color;

int main(int argc, char *argv[])
{
  char name[64]; int pid, cmd, i;

  while(1){
    pid = getpid();
    color = 0x0C;

    printf("----------------------------------------------\n");
    printf("     This is u2 binary\n");
    printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());

    // /show the passed in arguments.
    printf("argc = %d\n", argc);
    for(i = 0; i < argc; i++)
    {
      printf("argv[%d] = %s\n", i, argv[i]);
    }

    show_menu();
    printf("Command ? ");
    gets(name);
    if (name[0]==0)
        continue;

    cmd = find_cmd(name);
    switch(cmd){
           case 0 : getpid();   break;
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : fork();    break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;
           case 6 : exit();     break;
           case 9 : exec();     break;
           default: invalid(name); break;
    }
  }

  return 0;
}
