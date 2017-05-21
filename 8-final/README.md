Final
======================


460 Last LAB ASSIGNMENT

Chapter 13: User Interface

                     LAST LAB ASSIGNMENT
                   DUE and DEMO: CLOSE WEEK

1. MTX in ~samples/LAST/

1.1. Download all the files.
   vdisk is a flat IDE HD image with 4 partitions, each has a different version
   of MTX.

   Partition   MTX version
  ---------------------------------------------------------------------------
     1         rmtx     MTX in 16-bit real mode
     2         pmtx     MTX in 32-bit protected mode using dynamic paging
     3         smp      SMP_MTX in protected mode supporting up to 16 CPUs
     4         mtx32.1  MTX in protected mode using segmentation
 -----------------------------------------------------------------------------

   !!! SAVE A COPY OF vdisk IN CASE YOU TRASHED IT, or download it again

1.2. Read the script q, which is for running MTX from vdisk.
     To run q on vdisk: you need

       /dev/pts/1 and /dev/pts/2 for serail ports, /dev/pts/3 for parinter
       Optional: an .iso file as a virtual CDROM

1.3. Read the mku script to see how to generate a NEW user mode command
     Example:  mku test ===> generate user mode command from test.c in rmtx.

2. Syscalls in MTX Kernel

   ====================== MTX System Call Functions ======================
   Number  Name          Usage                         Function  
   ------ ---------  ---------------------------  ------------------------
     0    getpid       getpid()                    get process pid  
     1    getppid      getppid()                   get parent pid
     2    getpri       getpri()                    get priority
     3    setpri       setpri(pri)                 set priority
     4    getuid       getuid()                    get uid
     5    chuid        chuid(uid,gid)              set uid,gid
     6    yield        yield()                     switch process
     9    exit         exit(value)                 terminate process
    10    fork         fork()                      fork child process
   *11    exec         exec(cmd_line)              change image to a file
    12    wait         wait(&status;)               wait for child to die
    13    vfork        vfork()                     fork child process
    14    thread       thread(fd,stack,flag,prt)   create thread
    15    mutex_creat   mutex_creat()
    16    mutex_lock    mutex_lock(&mutex;)
    17    mutex_unlock  mutex_unlock(&mutex;)
    18    mutex_destroy mutex_destroy(&mutex;)
   ----------------------------------------------  -----------------------
    20    mkdir        mkdir(pathname)             make directory
    21    rmdir        rmdir(pathname)             rm   directory
    22    creat        creat(pathname)             creat file
    23    link         link(oldname, newname)      hard link to file
    24    unlink       unlink(pathname)            unlink
    25    symlink      symlink(oldname,newname)    create symbolic link
    26    readlink     readlink(name, buf[ ])      read symlink
    27    chdir        chdir(pathname)             change dir
    28    getcwd       getcwd(buf[ ])              get cwd pathname
    29    stat         stat(filename, &stat;_buf)    stat file
    30    fstat        fstat(fd, &stat;_buf)        stat file descriptor
    31    open         open(filename, flag)        open file for R|W|APP
    32    close        close(fd)                   close file descriptor
   *33    lseek        lseek(fd, position)         lseek
    34    read         read(fd, buf[ ], nbytes)    read file
    35    write        write(fd,buf[ ], nbytes)    write to file
    36    pipe         pipe(pd[ ])                 carete pipe
    37    chmod        chmod(filename, mode)       change permission
    38    chown        chown(filname, uid)         change file owner
    39    touch        touch(filename)             change file time
    40    settty       settty(tty_name)            set proc.tty name
    41    gettty       gettty(buf[ ])              get proc.tty name
    42    dup          dup(fd)                     dup file descriptor
    43    dup2         dup2(fd1, fd2)              dup fd1 to fd2
    44    ps           ps()                        ps in kernel
    45    mount        mount(FS, mountPoint)       mount file system
    46    umount       umount(mountPoint)          umount file system
    47    getSector    getSector(sector, buf[ ])   read CDROM sector
    48    cd_cmd       cd_cmd(cmd)                 issue cmd to CD driver
  -----------------------------------------------  -----------------------
    50    kill         kill(sig#, pid)             send signal to pid
    51    signal       signal(sig#, catcher)       install siganl handler
    52    pause        pause(t)                    pause for t seconds
    53    itimer       itimer(sec, action)         set timer request
    54    send         send(msg, pid)              send msg to pid
    55    recv         sender=recv(msg)            receive msg
    56    tjoin        tjoin(n)                    threads join
    57    texit        texit(value)                tthread exit
    58    hits         hits()                      I/O buffer hit ratio
    59    color        color(v)                    change display color
    60    sync         sync()                      sync file system
  ==========================================================================


    MTX syscalls are organized into 4 groups. Group 1 (0-19) is for process
management. Group 2 (20-49) is for file system operations. Group 3 is for
signals and signal processing, and Group 4 is for miceseleous system calls. All
the syscall functions in Groups 1-3 are compatible with those of Unix/Linux.

The only exception is exec(cmd_line). In Unix, when a user enters a command line
         a.out  arg1  arg2 ... argn
it is used in execl() as execl(a.out, a.out, arg1, arg2,...,argn, 0); or as
argc, argv[] in          execv(argc, argv);

In MTX, the entire command line is used in the exec() call. For example, if you
enter cat filename to the MTX sh, the child sh uses

          exec("cat filename");

to change its execution image to the cat program. However, the entire command
line is passed to the new image when execution starts. Parsing the command line
into argc and argv[] is done by a C start up code, crt0, in the new image.

3. Operation of the MTX system:
   The HD partition of MTX is an EXT2 FS containing

            /---|
                |----bin/ : All binary executables are in /bin
                |
                |---dev/ : special files tty0  c 4 0
                |                        ttyS0 c 5 0
                |                        ttyS1 c 5 1   
                |                        lp0   c 6 0
                |---etc/ : passwd file
                |           
                |---boot/: bootable MTX images
                |
                |---user/: users HOME directories

  After mounting the root file system, P0 creats P1, whose Umode image is
  the /bin/init program. P1 will go Umode directly to execute /bin/init,
  in which it forks children P2 on /dev/tty0. If the VM supports serial ports,
  P1 may also fork P3 /dev/ttyS0 and P4 on /dev/ttyS1. Then P1 waits for any
  child to die. Henceforth, P1 plays the role of the INIT process (P1) in Unix.

                          Proc#2:
  (1). P2 is a login process. It executes /bin/login on the console terminal
       /dev/tty0. So the special file /dev/tty0 MUST exist. To support serial
       terminals and printer, /dev/ttyS0, /dev/ttyS1 and /dev/lp0 must also
       exist.

  (2). In /bin/login, P2 opens its tty special file (/dev/tty0) as stdin(0),
       stdout(1) and stderr(2). Then it displays (to its stdout)
                  login:

  and waits for a user to login. When a user tries to login, it reads the
  user name and password (from its stdin), opens the /etc/passwd file to
  authenticate the user. Each line of /etc/passwd has the format:

          username:password:gid:uid:fullname:HOMEDIR:program
  e.g.    root:xxxxxxx:1000:0:superuser:/root:sh

  (Use plain text for password OR devise your own encryption schemes)

  If the user has a valid account in /etc/passwd, P2 becomes the user's process
  (by taking on the user's uid). It chdir to user's HOMEDIR and execute the
  listed program, which is normally the sh (/bin/sh).

  (3). then (in sh) it loops forever (until "logout" or Contro-D):
        {
           prompts for a command line, e.g. cmdLine="cat filename"
           if (cmd == "logout")
              syscall to die;

           // if just ONE cmd:  
           pid = fork();
           if (pid==0)
               exec(cmdLine);
           else
               pid = wait(&status;);
        }    

  When the child proc terminates (by exit() syscall to MTX kernel), it wakes up
  sh, which prompts for another cmdLine, etc.

 (4). When sh dies, it wakes up its parent, INIT, which forks another login
      process.         
==========================================================================

4. OBJECTIVES:

   The purpose of this assignment is for you to write YOUR OWN
   init, login, sh and other user command programs. Among these, sh is the
   most important. Accordingly, it will carry the most weight.

=============================================================================

                      5. ASSIGNMENTS:
                     DUE in CLOSED week
                 Demo and INTERVIEW during DEMO
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
5-1. Develop YOUR OWN programs for
             init                     // for process P1
             login                    // for login processes
             cat   [filename]         // cat filename or stdin to stdout
             more  [filename]         // more as in Unix                
             grep  pattern [filename] // print lines containing pattern
             l2u   [f1 f2]            // convert lower case to upper case
             cp    src dest           // src may be a file or DIR

     NOTE: [filename] means OPTIONAL.
           If no filenames, use stdin for IN and stdout for OUT

5-2. Write YOUR OWN sh.c to support I/O redirections and pipes:

     Examples:  cat [filename]        // NOTE: cat may NOT need a filenmae
                cat [filename] >  newfile
                cat [filename] >> appendFile                           
                a.out < inFile        // read inputs from inFile
                cat filename | more
                cat filename | grep test
                cat filename | l2u | grep PRINT

                cat filename > /dev/lp0   // print filename
                cp  filename /dev/lp0     // print filename

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


//*************************************************************************
//                      Logic of init.c
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1..
//************************************************************************

int pid, child, status;
int stdin, stdout;

#include "ucode.c"  <========== AS POSTED on class website

main(int argc, char *argv[])
{
  1. // open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages

  2. // Now we can use printf, which calls putc(), which writes to stdout
     printf("KCINIT : fork a login task on console\n");
     child = fork();

     if (child)
         parent();
     else             // login task
         login();
}       

int login()
{
  exec("login /dev/tty0");
}

int parent()
{
  while(1){
    printf("KCINIT : waiting .....\n");

    pid = wait(&status;);

    if (pid == child)
        fork another login child
    else
        printf("INIT : buried an orphan child %d\n", pid);
  }
}


//***********************************************************************
//                   LOGIC of login.c file
//***********************************************************************
char *tty;

main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
  tty =  argv[1];

 1. close(0); close(1); close(2); // login process may run on different terms

 2. // open its own tty as stdin, stdout, stderr

 3. settty(tty);   // store tty string in PROC.tty[] for putc()

  // NOW we can use printf, which calls putc() to our tty
  printf("KCLOGIN : open %s as stdin, stdout, stderr\n", tty);

  signal(2,1);  // ignore Control-C interrupts so that
                // Control-C KILLs other procs on this tty but not the main sh

  while(1){
    1. show login:           to stdout
    2. read user nmae        from stdin
    3. show passwd:
    4. read user passwd

    5. verify user name and passwd from /etc/passwd file

    6. if (user account valid){
          setuid to user uid.
          chdir to user HOME directory.
          exec to the program in users's account
       }
       printf("login failed, try again\n");
  }
}
==========================================================
                   sh Program:
YOUR sh must support
(1). logout/exit :  terminate
(2). simple command exceution, e.g. ls, cat, ....
(3). I/O redirection:  e.g. cat < infile;  cat > outfile, etc.
(4). (MULTIPLE) PIPEs: e.g. cat file | l2u | grep LINE
===========================================================
