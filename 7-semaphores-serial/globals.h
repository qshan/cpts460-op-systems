/*********************************************************************
*   Proccess
*********************************************************************/
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;
int body();

/*********************************************************************
*   String consts
*********************************************************************/
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter",
               "Saturn", "Uranus", "Neptune" };
char *delimiter = "----------------------------------------------------";
char *statuses[] = {"FREE", "READY", "RUNNING", "STOPPED", "SLEEP", "ZOMBIE"};

/*********************************************************************
*   Video
*********************************************************************/
int color;
int rflag;
/*********************************************************************
*   Pipes
*********************************************************************/
OFT oft[NOFT];
PIPE pipes[NPIPES];

u16 vector, addr;
