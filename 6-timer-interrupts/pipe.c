
/*
1. Add a file descriptor array, fd[NFD], to every PROC structure:
  typedef struct proc{
    // same as before
    OFT *fd[NFD]; // opened file descriptors; copy on fork() int kstack[SSIZE];
  }PROC;

2. Allocate a PIPE object. Initialize the PIPE object with
  head=tail=0; data=0; room=PSIZE; nreaders=nwriters=1;

3. Allocate 2 OFTs. Initialize the OFTs as
  readOFT.mode = READ_PIPE; writeOFT.mode = WRITE_PIPE;
  both OFT's refCount=1 and pipe_ptr points to the same PIPE object.

4. Allocate 2 free entries in the PROC.fd[] array,e.g.fd[i] and fd[j].
  Let fd[i] point to readOFT and fd[j] point to writeOFT.

5. write index i to pd[0] and index j to pd[1]; both are in Uspace.

6. return 0 for OK;
*/

int getFreePipeIndex()
{
  int i;
  for (i = 0; i < NPIPES; i++) {
    if (pipes[i].nreader == 0 && pipes[i].nwriter == 0) {
      return i;
    }
  }
}

int getFreeOFTIndex()
{
  int i;
  for (i = 0; i < NPIPES; i++) {
    if (oft[i].refCount == 0) {
      return i;
    }
  }
}

int getFreeFDIndex()
{
  int i;
  for (i = 0; i < NFD; i++) {
    if (running->fd[i] == 0 || running->fd[i]->refCount == 0) {
      return i;
    }
  }
}

printOFT(int index)
{
  printf("%s\n", delimiter);
  printf("OFT[%d]\n", index);
  printf("%s\n", delimiter);
  printf("buf = %s\n", oft[index].pipe_ptr->buf);
  printf("%s\n", delimiter);
  printf("readOFT refCount = %d, readers = %d, writers = %d\n", oft[index].refCount, oft[index].pipe_ptr->nreader, oft[index].pipe_ptr->nwriter);
  printf("readOFT data = %d, room = %d, head = %d, tail = %d\n", oft[index].pipe_ptr->data, oft[index].pipe_ptr->room, oft[index].pipe_ptr->head, oft[index].pipe_ptr->tail);
  printf("%s\n", delimiter);
}

int kpfd()
{
  int i = 0, count = 0;
  char statstr[12];
  char *stat = &statstr;
  //WRITE C code to print PROC information
  printf("%s\n", delimiter);
  printf("%s\n", delimiter);
  printf("INDEX\tMODE\tREFCOUNT\n");
  for(i = 0; i < NFD; i++){
    if(running->fd[i])
    {
      switch(running->fd[i]->mode){
        case(1): strcpy(statstr,"READ_PIPE");   break;
        case(2): strcpy(statstr,"WRITE_PIPE");  break;
        default: strcpy(statstr,"NA");       break;
      }
      printf("%d\t%s\t%d\n", i, statstr, running->fd[i]->refCount);
      count++;
    }
   }
   if (count == 0) {
     printf("\tno open files\n");
   }
   printf("%s\n", delimiter);
  // printf("I am proc %d in UMODE: running segment=%x\n", running->pid, running->uss);
  return 0;
}

int kpipe(int *pd)
{
  unsigned short i, j;
  PIPE *pipe;
  OFT *readOFT, *writeOFT;
  printf("Entered kpipe\n");

  //get and init pipe
  i = getFreePipeIndex();
  printf("init pipe at index %d\n", i);
  pipe = &pipes[i];
  pipe->head = 0;
  pipe->tail = 0;
  pipe->data = 0;
  pipe->room = PSIZE;
  pipe->nreader = 1;
  pipe->nwriter = 1;
  //get and init OFTs
  i = getFreeOFTIndex();
  printf("init readOFT at index %d\n", i);
  readOFT = &(oft[i]);
  readOFT->refCount = 1;
  readOFT->mode = READ_PIPE;
  i = getFreeOFTIndex();
  printf("init writeOFT at index %d\n", i);
  writeOFT = &(oft[i]);
  writeOFT->refCount = 1;
  writeOFT->mode = WRITE_PIPE;
  //pipr pointers
  readOFT->pipe_ptr = pipe;
  writeOFT->pipe_ptr = pipe;
  printf("grabbed oft and pipe\n");

  //set ofts in proc
  i = getFreeFDIndex();
  printf("init read fd p->fd[%d]\n", i);
  running->fd[i] = readOFT;
  j = getFreeFDIndex();
  printf("init write fd p->fd[%d]\n", j);
  running->fd[j] = writeOFT;
  printf("writing %d and %d to usermode segment %x\n", i, j, running->uss);
  put_word(i, running->uss, pd);
  put_word(j, running->uss, pd + 1);
  return 0;

}


int kclosepipe(int closefd)
{
  OFT *closeoft;
  //verify that fd exists
  if(!running->fd[closefd])
  {
    printf("there is no fd at %d\n", closefd);
    return -1;
  }
  //grab the oft 
  closeoft = running->fd[closefd];
  //clear it from proc
  running->fd[closefd] = 0;

  //decrement refcount
  closeoft->refCount--;
  //closing the read oft
  if (closeoft->mode == READ_PIPE) {
    //decrement the reader count
    closeoft->pipe_ptr->nreader--;
    //last reader on pipe?
    if(closeoft->pipe_ptr->nreader == 0)
    {
      //both reader and writer gone?
      if (closeoft->pipe_ptr->nwriter == 0) {
        //deallocate pipe
        memset(closeoft->pipe_ptr->buf, 0, PSIZE);
        closeoft->pipe_ptr->head = closeoft->pipe_ptr->tail = 0;
        closeoft->pipe_ptr->data = 0;
        closeoft->pipe_ptr->room = PSIZE;
        closeoft->pipe_ptr->nreader = closeoft->pipe_ptr->nwriter = 0;
      }
      //deallocate readOFT
      closeoft->mode = READ_PIPE;
      closeoft->refCount = 0;
      closeoft->pipe_ptr = 0;
    }
    //wakeup blocked readers
    kwakeup(&closeoft->pipe_ptr->data);
  }
  //closing the write oft
  else {
    //decrement the writer count
    closeoft->pipe_ptr->nwriter--;
    //last writer on pipe?
    if(closeoft->pipe_ptr->nwriter == 0)
    {
      //both reader and writer gone?
      if (closeoft->pipe_ptr->nreader == 0) {
        //deallocate pipe
        memset(closeoft->pipe_ptr->buf, 0, PSIZE);
        closeoft->pipe_ptr->head = closeoft->pipe_ptr->tail = 0;
        closeoft->pipe_ptr->data = 0;
        closeoft->pipe_ptr->room = PSIZE;
        closeoft->pipe_ptr->nreader = closeoft->pipe_ptr->nwriter = 0;
      }
      //deallocate readOFT
      closeoft->mode = READ_PIPE;
      closeoft->refCount = 0;
      closeoft->pipe_ptr = 0;
    }
    //wakeup blocked writers
    kwakeup(&closeoft->pipe_ptr->room);
  }

  closeoft = 0;
  return 0;

}


int kreadpipe(int fd, char *buf, int n)
{
  int i = 0, ret = 0;
  OFT *pipoft;
  PIPE *pipe;
  printf("enter readpipe\n");
  //guard
  if (n <= 0) {
    printf("n <= 0 nothing to read\n");
    return 0;
  }
  if(!running->fd[fd] || running->fd[fd]->mode != READ_PIPE)
  {
    printf("invalid fd at %d\n", fd);
    return 0;
  }
  //grab oft and pipe
  pipoft = running->fd[fd];
  pipe = pipoft->pipe_ptr;
  printf("grabbed oft and pipe\n", fd);
  printOFT(fd);
  //printf("starting main loop\n");
  //while we havent hit our limit
  while(i < n)
  {
    //printf("while theres data in pipe\n");
    //while there is data to read
    while(pipe->data)
    {
      //put byte in userspace buf with
      //printf("putting byte: %c to umode", pipe->buf[pipe->tail]);
      put_byte(pipe->buf[pipe->tail++], running->uss, buf + i);
      //wrap round circular buf
      if (pipe->tail == PSIZE) {
        pipe->tail = 0;
      }
      //counter management
      i++; ret++; pipe->data--; pipe->room++;
      if(i == n){ break; }
    }
    //printf("after reading ret = %d\n", ret);
    //we read info
    if(ret)
    {
      printf("wakeup writers with room, read %d bytes\n", ret);
      kwakeup(&pipe->room);
      return ret;
    }
    //we still have a writer
    if(pipe->nwriter)
    {
      printf("wakeup writers with room, sleeping for data\n");
      kwakeup(&pipe->room);
      ksleep(&pipe->data);
      continue;
    }
    printf("why are we here!\n");
    return 0;
  }
}


int kwritepipe(int fd, char *buf, int n)
{
  int i = 0, ret = 0;
  OFT *pipoft;
  PIPE *pipe;
  printf("enter writepipe\n");
  //guard
  if (n <= 0) {
    printf("n <= 0 nothing to write\n");
    return 0;
  }
  if(!running->fd[fd] || running->fd[fd]->mode != WRITE_PIPE)
  {
    printf("invalid fd at %d\n", fd);
    return 0;
  }
  //grab oft and pipe
  pipoft = running->fd[fd];
  pipe = pipoft->pipe_ptr;
  //printf("grabbed oft and pipe\n", fd);
  printOFT(fd);
  //printf("starting main loop\n");
  //while we havent hit our limit
  while(i < n)
  {
    if (pipe->nreader == 0) {
      printf("BROKEN_PIPE\n");
      kexit(13);
    }
    while (pipe->room > 0) {
      //printf("putting one byte to buff\n");
      pipe->buf[pipe->head++] = get_byte(running->uss, buf + i);
      //wrap round circular buf
      if (pipe->head == PSIZE) {
        printf("pipe head wrapped\n");
        pipe->head = 0;
      }
      i++; ret++; pipe->data++; pipe->room--;
      if(i == n){ printf("finished writing %d bytes\n", i); break; }
    }
    printf("waking up readers\n");
    kwakeup(&pipe->data);
    if(i == n ){printf("wrote %d bytes needed to buf\n", ret);   printOFT(fd); return ret; }
    printf("sleeping for more room\n");
    ksleep(&pipe->room);
  }
  printf("why are we here?\n");
  return ret;
}
