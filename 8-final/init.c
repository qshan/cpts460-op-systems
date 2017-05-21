//*************************************************************************
//                      Logic of init.c
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1..
//************************************************************************

int pid, console, ser1, ser2, status;
int stdin, stdout;

#include "ucode.c"  //<========== AS POSTED on class website

main(int argc, char *argv[])
{
  //close the normal FD's
  close(0); close(1);

  //1. // open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
  stdin = open("/dev/tty0", O_RDONLY);
  stdout = open("/dev/tty0", O_WRONLY);
  // TODO: works without these lines but i'm not sure why
  // dup2(stdin, 0);
  // dup2(stdout, 1);


  //2. // Now we can use printf, which calls putc(), which writes to stdout
  printf("INIT : fork a login task on console\n");
  console = fork();
  //if fork returns 0 we are the child. start login
  if (!console)
  {
    login();
  }

  printf("INIT : fork a login task on serial 0\n");
  ser1 = fork();
  //if fork returns 0 we are the child. start login
  if (!ser1)
  {
    loginser1();
  }

  printf("INIT : fork a login task on serial 1\n");
  ser2 = fork();
  //if fork returns 0 we are the child. start login
  if (!ser2)
  {
    loginser2();
  }

  //NEVER SAY DIE!
  while(1)
  {
    pid = wait(&status);
    //Console process died
    if (pid == console) {
      printf("INIT : console child died, fork another\n");
      console = fork();
      if (!console)
      {
        login();
      }
      continue;
    }
    //serial one died
    if (pid == ser1) {
      printf("INIT : serial 0 child died, fork another\n");
      ser1 = fork();
      if (!ser1)
      {
        loginser1();
      }
      continue;
    }
    //serials 2 died
    if (pid == ser2) {
      printf("INIT : serial 1 child died, fork another\n");
      ser2 = fork();
      if (!ser2)
      {
        loginser2();
      }
      continue;
    }
  }
}

int login()
{
  exec("login /dev/tty0");
}

int loginser1()
{
  exec("login /dev/ttyS0");
}

int loginser2()
{
  exec("login /dev/ttyS1");
}
