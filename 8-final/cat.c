#include "ucode.c"


char buf[1024];
char *bp;


int main(int argc, char *argv[])
{
  char ch;
  int fd = 0, n = 1, i, nbytes;
  //bad input?
  if(argc > 2){ printf("USAGE: cat [filename]\n"); return -1;}
  //do we have a file?
  if(argc == 2)
  {
    //printf("argc: %d, file: %s\n", argc, argv[1]);
    //redirect our input from the file
    //close(0);
    fd = open(argv[1], O_RDONLY);
    nbytes = 1024;
    if(fd < 0)
    {
      printf("error opening file: %s\n", argv[1]);
      return -1;
    }
  }
  else{
    fd = 0;
    nbytes = 1;
  }

  while(n > 0)
  {
    //reset
    memset(buf, 0, 1024);
    i = 0;
    //printf("reading from fd %d ", fd);
    //read from input
    n = read(fd, buf, nbytes);
    //printf("read %d bytes\n", n);
    while (i != n)
    {
      // TODO: since theres no "newline" from stdin, but it breaks when piping/redirecting input
      // this causes extra newlines when working in a pipe but works correctly for stdin.
      // if(buf[i] == '\r' && strncmp(buf[i], "\r\n", 2))
      // {
      //   printf("\n");
      // }
      printf("%c", buf[i++]);
    } //while over current buffer
  } // while we read data
}
