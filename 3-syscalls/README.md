Syscalls
======================


460 Lab Assignment #3

                     460 LAB Assignment #3
                     DUE: Week of 9-19-2016

1. OBJECTIVES:
   Kernel/User mode images and system calls

2. REQUIREMENTS:

   Implement the following system call functions for the MTX system:
   For each syscall, return (int) value >= 0 for OK, or -1 for BAD.
***************************************************************************
(0). pid = getpid()  : return the proc's pid

(1). ps()            : enter Kerenl to print the status info of the procs

(2). chname(char *s) : ask for a string *s, enter Kernel to change the
                       running proc's name to *s.
(3). kfork()         : enter Kernel to kfork a child with /bin/u1 as Umode image

(4). kswitch()       : enter Kernel to switch process (call tswitch()).

(5). pid = wait(int *exitValue) : enter Kernel to wait for a ZOMBIE child,
                                  return its pid (or -1 if no child) and its
                                  exitValue.
(6). exit(int exitValue) : enter Kernel to die with an exitValue.
==========================================================================
(7). getc()/putc() as syscalls to Kernel.

***************************************************************************

                    3. HELPS INFO

MUST READing MATERIAL:

TEXT: Chapter 5: Sections 5.3, 5.4, 5.5, 5.6, 5.7



POSTED FILES:
                    samples
                      |
                     LAB3.post
            ----------------------------------------------
             |              |        |   |    |         |
            USER          mtxlib     mk t.c ts.s  Other C files
        ------------    (loader.o)  
       mku, u.s, u1.c

=============================================================
      mtxlib is a link LIBRARY containing
             load("filename", segment);
      which loads filename into segment for execution
============================================================

                          MTX Kernel:

The entry point of  INT 80 handler is (in ts.s file)

       _int80h:
        SAVE:    ! save registers in ustack; switch to Kernel space;
                 ! save uSS, uSP in proc

                 call _kcinth      ! call int kcinth() in C; fix return value

        _goImode:! restore SS, SP from saved uSS, uSP;
                 ! pop ustack to restore CPU registers
                 ! IRET        return to where INT 80 was issured in Umode

============================================================================
4. DEMO PROGRAM:
                  samples/LAB3.post/mtximage
   download and run it as a virtual FD for QEMU.
============================================================================

                  460 LAB #3 HELPs

In your main() function:

1. initialize kernel data structures as in LAB#3

2. Set Vector 80:

        int int80h();    // tell C compiler inth80 is a function
                         // _int80h: is in assembly code
        // Use put_word(word, segment, offset) to set

        vector80 = [PC, CS] = [int80h, kernel_segment]


3. Modify YOUR kfork() function to

   PROC *kfork(char *filename)
   {
     // kfork as BEFORE: pid = new PROC's pid

     new PROC's segment = (pid+1)*0x1000;

     if (filename){
         load filename to segment by load(filename, segment);

                  READ Chapter 5.6, 5.7 !!!!
===============================================================================
(3).1    SETUP new PROC's ustack for it to return to Umode to execute filename;
(3.2).   Set new PROC.uss = segment;
                 PROC.usp = TOP of ustack (per INT 80)

(3).1 and (3.2) ACCOMPLISH THE FOLLOWING DIAGRAM:         

       new PROC
        ------
        |uss | = new PROC's SEGMENT value
        |usp | = -24                                    
        --|---                                    Assume P1 in segment=0x2000:
          |                                              0x30000  
          |                                              HIGH END of segment
  (LOW) | |   ----- by SAVE in int80h ----- | by INT 80  |
  --------|-------------------------------------------------------------
        |uDS|uES| di| si| bp| dx| cx| bx| ax|uPC|uCS|flag|NEXT segment
  ----------------------------------------------------------------------
         -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1 |

Use put_word(w, segment, offset) to set the ustack contents to contain:

         uDS=segment, uES=segment,          // MUST be the segment
         di, si, bp, dx, cx, bx, ax = 0     // CPU registers are all 0
         uPC = 0                            // (uCP, uPC) = VRITUAL address 0
         uCS = segment
         flag=0x0200 (I bit-1 for allow interrupts)
===============================================================================
     }

     return pointer to new PROC;
   }

5. Let P0 create P1 by
          kfork("/bin/u1");
   P1 will have an Umode image /bin/u1 loaded in the segment 0x2000.

6. Let P0 tswitch to P1. P1 resumes to body() AS BEFORE

7. In body() function: ADD a 'u' command

      case 'u': goUmode();  break;   // let process goUmode
-----------------------------------------------------------------------

8. Write YOUR first version of u1.c file as


#include "ucode.c"
int color;

main()
{
   printf("I am in Umode segment=%s\n", getcs());
   while(1);
}

9. If you see the above message after 'u' command,
   your P1 has goUmode successfully.


10. Then use the given u1.c file in USER/ to generate new u1 image.
    Then try to do syscalls 0 to 5.
    Lastly, implement getc()/putc() as syscalls to kernel.
===============================================================

11. (REQUIRED): Modify YOUR booter to be a

           int loader(char *filename, int segment)

 which loads filename's |CODE|DATA| to segment AND clear its BSS section to 0

 NOTE: a.out has a 32-byte file header containig 8 u32 entries.

                    | u32  |tsize|dsize|  bsize  |   
       a.out file = |header|CODE |DATA |...bss...|

where header is a structure

      struct header{
         u32 type;          
         u32 pad;
         u32 tsize;        // CODE size in bytes
         u32 dsize;        // DATA size in bytes
         u32 bsize;        // bss  size in bytes
         u32 zero;
         u32 totalSize
         u32 symtableSize;
      }
================================================================
