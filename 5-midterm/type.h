/******* typedefs ********/
typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;
/******* PROC Consts ********/
#define NPROC    16
#define SSIZE 1024
/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5
/******* Variable seg size! ********/
//  0x1000 = 4096 bytes
//  64kb = 65536 bytes = 0x10000
//  from 0x0000 - 0xffff = 65535 bytes
//  top 16 bits of 20 bit rep of SEGS?
// Segment size in words
// 32 * 1024 = 32768
// 16 * 1024 = 16384
#define SEGSIZE 0x800
/******* PIPES Consts ********/
#define READ_PIPE 1
#define WRITE_PIPE 2
#define NFD 10
#define NOFT 10
#define PSIZE 64
#define NPIPES 10

/******* Structs ********/
typedef struct pipe{
  char buf[PSIZE];      // circular data buffer
  int head, tail;       // circular buf index
  int data, room;       // number of data & room in pipe
  int nreader, nwriter; // number of readers, writers on pipe
}PIPE;

typedef struct oft{
    int mode;               // READ_PIPE|WRITE_PIPE
    int refCount;           // number of PROCs using this pipe end
    struct pipe *pipe_ptr;  // pointer to the PIPE object
}OFT;

typedef struct proc{
    struct proc *next;
    int    *ksp;               // at offset 2

    int    uss, usp;           // at offsets 4,6
    int    inkmode;            // at offset 8

    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];           // name string of PROC
    OFT    *fd[NFD];
    int    kstack[SSIZE];      // per proc stack area
}PROC;
