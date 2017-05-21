#include "ucode.c"


char buf[1024];
char *bp;

//TODO: Reads 0 bytes from pipes and closes, thereby breaking the pipe.

int main(int argc, char *argv[])
{
  char ch, *pattern;
  int bufindex = 0, lineindex = 0, search = 0, patlen;
  int fd = 0, n = 1, i, nbytes;
  //bad input?
  if(argc > 3){ printf("USAGE: grep pattern [filename]\n"); return -1;}
  //do we have a file?
  if(argc == 3)
  {
    //printf("argc: %d, file: %s\n", argc, argv[1]);
    //redirect our input from the file
    //close(0);
    fd = open(argv[2], O_RDONLY);
    nbytes = 1024;
    if(fd < 0)
    {
      printf("error opening file: %s\n", argv[2]);
      return -1;
    }
  }
  else //read from stdin
  {
    fd = 0;
    nbytes = 1024;
  }
  // grab pattern gor later
  pattern = argv[1];
  patlen = strlen(pattern);

  //printf("grep %s for pattern %s\n", argv[2], pattern);
  while(n > 0)
  {
    //reset
    memset(buf, 0, 1024);
    bufindex = 0;
    lineindex = 0;
    //printf("reading from fd %d ", fd);
    //getc();
    //read from input
    n = read(fd, buf, nbytes);
    //printf("read %d bytes\n", n);
    while (bufindex < n)
    {
      lineindex = 0;
      memset(line, 0, 64);
      //copy a line to the lin buffer
      while(buf[bufindex] != '\n' && buf[bufindex] != '\r' && i < n && lineindex < 64)
      {
        line[lineindex++] = buf[bufindex++];
      }
      //we need more DATA
      if(bufindex == n){continue;}
      //nullterm the line
      line[lineindex] = 0;

      if(line[0] == 0){ bufindex++;}

      //printf("Read line = %s\n", line);
      //WE have a line in line[]
      //search every substring for match
      search = 0;
      while(search < lineindex)
      {
        //search line for pattern
        //printf("compare %d chars |%s| to |%s|\n", patlen, &(line[search]), pattern );
        if(!strncmp(&(line[search++]), pattern, patlen))
        {
          //if matched print the line
          printf("%s\n", line);
          //printf("lineindex = %d, bufindex = %d\n", lineindex, bufindex);
          //getc();
          break;
        }
      }
    } // while over current buffer
  } // while we have read data
  //printf("grep exiting\n");
}
