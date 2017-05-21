Timer and Interrupts
======================

CS460 Assignment #7: Timer
           DUE: Friday: After Spring Brreak
           Demo Time: 12:00-3:00pm and 4:00pm-7:00pm on Friday(03/25/2016)
             (The sign-up sheet is now available outside of the TA's office.)
In the TEXT: Chpater 8 contains a timer dirver, which displays
       "one second timer interrupt in K or U mode"
on each second.

MODIFY thandler() to do the following:


(1).At each second : Display current time HH:MM:SS at the lower right corner.
    Read my video display driver vid.c code for HOW.

(2).When a proc is scheduled to run, set its PROC.time to a time-slice value,
    e.g. 5 seconds.
    On each second, decrement running's time in Umode only!  When a proc's run
    time expires, switch process.
         NOTE:   do NOT switch process while it's in Kmode!!!
         REASON: our MTX kernel is NOT a multi-processor kernel.

    In order to do (2), modify u1.c code as follows:

         main()
         {
           int i,j;
           get its pid and ppid
           while(1){
             print its pid and ppid
             add a LARGE delay loop here;
           }
         }

     so that every Umode process just loops in UMODE.

In your t.c file: let P0 kfork("/bin/u1") several times to create
        P1, P2, P3, P4
all run in Umode directly. Then tswitch to P1. YOUR TIMER should switch
processes every 5 seconds.


(3). From Umode, add a command, sleep, which ask for a time t to sleep.
     Syscall to MTX kernel to sleep for t seconds.
     The process becomes READY to run again when its sleep time expires.




=================== FUN projects you may try =======================

1. Poor-man's X-windows:

   Each process has a private
        char saved_sreen[4000];

   When switch process,
      . save the display memory into CURRENT proc's saved_screen[ ].
      . restore display memory from saved_screen[] of the NEXT proc.


2. Separate Display regions by procs

   Divide the screen (display memory) into REGIOGs, e.g. 4 regions, each is
   for a set of procs by pid

          ---------------------------------
          |               |               |
          |   (pid%4)=0   |  (pid%4)=1    |
          |               |               |
          |--------------------------------
          |               |               |
          |   (pid%4)=2   |  (pid%4)=3    |
          |               |               |
          ---------------------------------
