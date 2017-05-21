PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter",
               "Saturn", "Uranus", "Neptune" };
char *delimiter = "----------------------------------------------------";
char *statuses[] = {"FREE", "READY", "RUNNING", "STOPPED", "SLEEP", "ZOMBIE"};

int color;
int rflag;

OFT oft[NOFT];
PIPE pipes[NPIPES];
