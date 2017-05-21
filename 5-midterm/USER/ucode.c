// ucode.c file
#define SEGSIZE 0x800
int margc;
char arguments[16][32];
char *margv[16];
int pd[2];


int show_menu()
{
   printf("***************** Menu *************************\n");
   printf("*  ps  chname  fork  switch  wait  exec  exit  *\n");
   printf("*  hop  pipe   read  write   close  pfd        *\n");
   printf("************************************************\n");
}

int find_cmd(char *name)
{
  if(strcmp(name, "ps") == 0)
  {
    return 1;
  }
  else if(strcmp(name, "chname") == 0)
  {
    return 2;
  }
  else if(strcmp(name, "fork") == 0)
  {
    return 3;
  }
  else if(strcmp(name, "switch") == 0)
  {
    return 4;
  }
  else if(strcmp(name, "wait") == 0)
  {
    return 5;
  }
  else if(strcmp(name, "exit") == 0)
  {
    return 6;
  }
  else if(strcmp(name, "exec") == 0)
  {
    return 9;
  }
  else if(strcmp(name, "hop") == 0)
  {
    return 10;
  }
  else if(strcmp(name, "pipe") == 0)
  {
    return 11;
  }
  else if(strcmp(name, "read") == 0)
  {
    return 12;
  }
  else if(strcmp(name, "write") == 0)
  {
    return 13;
  }
  else if(strcmp(name, "close") == 0)
  {
    return 14;
  }
  else if(strcmp(name, "pfd") == 0)
  {
    return 15;
  }
  else{
    invalid(name);
    return -1;
  }
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int fork()
{
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid);
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
}

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n");
}

int atoi(char *s)
{
  int v = 0;
  while(*s){
    v = v*10 + (*s-'0');
    s++;
  }
  return v;
}

int geti()
{
  char s[16];
  gets(s);
  return atoi(s);
}

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}


int getc()
{
  return syscall(7,0,0,0);
}

int putc( char c)
{
  return syscall(8,c,0,0);
}

int exec()
{
  char s[32];
  printf("Enter exec filename : ");
  gets(s);

  printf("exec syscall for %s\n", s);
  return syscall(9,s,0,0);
}

int hop()
{
  unsigned short newseg;
  int input;
  printf("Enter segment to hop to : ");
  input = geti();

  newseg = SEGSIZE * input + 0x1000;

  printf("exec syscall for hop, seg %x\n", newseg);
  return syscall(10,newseg,0,0);
}



int pipe()
{
   printf("pipe syscall\n");
   syscall(11, pd, 0, 0);
   printf("proc %d created a pipe with fd = %d %d\n",
           getpid(), pd[0], pd[1]);
}

int pfd()
{
  syscall(15,0,0,0);
}

int read_pipe()
{
  char fds[32], buf[1024];
  int fd, n, nbytes;
  pfd();

  printf("read : enter <fd> <nbytes> : ");
  gets(fds);
  sscanf(fds, "%d %d",&fd, &nbytes);
  printf("fd=%d  nbytes=%d\n", fd, nbytes);

  n = syscall(12, fd, buf, nbytes);

  if (n>=0){
     printf("proc %d back to Umode, read %d bytes from pipe : ",
             getpid(), n);
     buf[n]=0;
     printf("%s\n", buf);
  }
  else
    printf("read pipe failed\n");
}

int write_pipe()
{
  char fds[16], buf[1024];
  int fd, n, nbytes;
  pfd();
  printf("write : enter <fd> <text> : ");
  gets(fds);
  sscanf(fds, "%d %s", &fd, buf);
  nbytes = strlen(buf);

  printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

  n = syscall(13,fd,buf,nbytes);

  if (n>=0){
     printf("\nproc %d back to Umode, wrote %d bytes to pipe\n", getpid(),n);
  }
  else
    printf("write pipe failed\n");
}

int close_pipe()
{
  char s[16];
  int fd;
  printf("enter fd to close : ");
  gets(s);
  fd = atoi(s);
  syscall(14, fd, 0, 0);
}










int  tokenize(char *source, char *dest[], char *delimiter)
{
  char str_buff[32];
  char *token;
  int i=0, total = 0;

  //LOG(LOG_INFO, "source: %s, delimiter: %s\n", source, delimiter);

  if(!source || !dest || !delimiter)
  {
    printf("bad inputs to tokenize\n");
    return -1;
  }
  strcpy(str_buff, source);

  //LOG(LOG_INFO, "Tokenizing string.\n");
  token = strtok(str_buff, delimiter);

  while(token)
  {
    // printf("token (%d) = %s\n", i, token );
    //dest[i] = (char *)malloc(sizeof(char) * (strlen(token)+1));
    strcpy(&(arguments[i][0]), token);
    margv[i] = &(arguments[i][0]);
    //printf("after copy margv[%d] = %s", i, margv[i]);
    total++;
    i++;
    token = strtok(0, delimiter);
  }
  margv[i] = 0;
  //printf("total = %d", total);
  return total;
}

main0(char *cmdline)
{
  int i;
  printf("in main0, user input = %s\n", cmdline);
  margc = tokenize(cmdline, margv, " ");
  // printf("tokenize complete margc = %d\n", margc);
  // for(i = 0; i < margc; i++)
  // {
  //   printf("margv[%d] = %s\n", i, margv[i]);
  // }


  main(margc, margv);
}
