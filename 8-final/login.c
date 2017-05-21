#include "ucode.c"

#define BUFSIZE 512

char *tty;
//tokenize
char passline[8][32], *tokens[8];
//getline
char buf[BUFSIZE], *bp;

int  tokenize(char *source, char *delimiter)
{
  char str_buff[64];
  char *token;
  int i=0, total = 0;

  if(!source || !delimiter)
  {
    printf("bad inputs to tokenize\n");
    return -1;
  }
  //strtok destrosy source
  strcpy(str_buff, source);
  //tokenize!
  token = strtok(str_buff, delimiter);
  while(token)
  {
    //printf("token (%d) = %s\n", i, token );
    //copy and save pointer
    strcpy(&(passline[i][0]), token);
    tokens[i] = &(passline[i][0]);
    total++; i++;
    //get next token
    token = strtok(0, delimiter);
  }
  //null term the next string
  //TODO: test to see whats up
  passline[i][0] = 0;
  //printf("total = %d", total);
  return total;
}

int getline(char *outbuf)
{
  int i=0, j=0;
  char *op = outbuf;

  //while we filled the buffer
  while(*bp)
  {
    //is it a newline char?
    if(*bp == '\n' || *bp == '\r')
    {
      //printf("GETLINE : hit newline char.\n");
      //null term
      bp++;
      *op = 0;
      //return the count
      return j;
    }
    //printf("GETLINE : copy char %c.\n");
    //copy it over
    *op = *bp;
    //move the pointers
    bp++; op++; j++;
  }
  //nulterm
  *op = 0;
  //read n < 512, return the count read
  return j;
}

int main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
  int stdin, stdout, stderr, passfile, n = 0, i;
  int uid, gid;
  char username[32], passwd[32], line[64], *cp;
  tty = argv[1];

  //1. close(0); close(1); close(2); // login process may run on different terms
  close(0); close(1); close(2);
  //2. // open its own tty as stdin, stdout, stderr
  stdin = open(tty, O_RDONLY);
  stdout = open(tty, O_WRONLY);
  stderr = open(tty, O_WRONLY);
  //3. settty(tty);   // store tty string in PROC.tty[] for putc()
  settty(tty);
  // NOW we can use printf, which calls putc() to our tty
  printf("LOGIN : open %s as stdin, stdout, stderr\n", tty);
  // ignore Control-C interrupts so that
  signal(2,1);  // Control-C KILLs other procs on this tty but not the main sh

  //open the passwd file, and grab a copy
  passfile = open("/etc/passwd", O_RDONLY);
  //if we can't open the file...we should probably stop.
  if(passfile < 0) { printf("LOGIN : could not open passwd file!\n"); return -1;}
  memset(buf, 0, BUFSIZE);
  n = read(passfile, buf, BUFSIZE);
  bp = buf;
  //if passwd is BIG we might not be reading the whole thing.
  if(n == BUFSIZE)
  {
    printf("LOGIN : passwd file might be truncated!\n");
  }
  close(passfile);

  //NEVER SAY DIE!...until exec
  while(1){
    //1. show login: to stdout
    printf("login: ");
    //2. read user name from stdin
    gets(username);
    //3. show passwd:
    printf("password: ");
    //4. read user passwd
    gets(passwd);

    //5. verify user name and passwd from /etc/passwd file
    //setup for read
    memset(line, 0, 64);
    cp = line;
    //read line at a time from "file"
    while(n = getline(line))
    {
      //printf("LOGIN : n = %d, line = %s \n", n, line);
      //tokenize line from passwd on ':', see passwd
      tokenize(line, ":");

      //6. if (user account valid)
      if( !strcmp(username, tokens[0]) && !strcmp(passwd, tokens[1]))
      {
        // printf("LOGIN : username = %s, tokens[0] = %s\n", username, tokens[0]);
        // printf("LOGIN : password = %s, tokens[1] = %s\n", passwd, tokens[1]);
        //setuid to user uid.
        uid = atoi(tokens[3]);
        gid = atoi(tokens[2]);
        // printf("LOGIN : uid = %d, gid = %d\n", uid, gid);
        chuid(uid, gid);
        //chdir to user HOME directory.
        // printf("LOGIN : homedir = %s\n", tokens[5]);
        chdir(tokens[5]);
        //exec to the program in users's account
        // printf("LOGIN : exec = %s\n", tokens[6]);
        exec(tokens[6]);
        printf("Welcome back %s!\n", username);
        break;
      }
    }
    //reset etc file buf
    bp = buf;
    printf("login failed, try again\n");
  }
}
