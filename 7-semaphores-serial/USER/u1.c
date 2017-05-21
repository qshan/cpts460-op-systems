#include "ucode.c"
int color;

int main(int argc, char *argv[])
{
  char name[64]; int pid, cmd, i, j, k;

  while(1){
    pid = getpid();
    printf("I am proc %d in U mode.\n",pid);

    // // Loop for delay to demonstrate time slicing
    // i=0;
    // while(i < 100000)
    // {
    //   // j = i/7;
    //   // k = i%7;
    //   // j += k;
    //   i++;
    // }

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
           case 3 : fork();     break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;
           case 6 : exit();     break;
           case 9 : exec();     break;
           case 10 : hop();     break;
           case 11 : pipe();    break;
           case 12 : read_pipe();    break;
           case 13 : write_pipe();    break;
           case 14 : close_pipe();    break;
           case 15 : pfd();    break;
           case 16 : itimer();    break;
           case 17 : sgets();     break;
           case 18 : sputs();     break;
           default: invalid(name); break;
    }
  }

  return 0;
}
