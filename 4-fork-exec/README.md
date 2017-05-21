Fork-exec
======================

460 Lab Assignment #4

                     460 LAB Assignment #4
                     DUE: Week of 9-26-2016

1. OBJECTIVES:
   fork and exec in MTX (behave the same as in Unix/Linux)

   READ: Chapter 5.9.1: fork in MTX
                 5.9.2: exec in MTX
                 5.10:  command-line parameters

2. REQUIREMENTS:

   Implement the following system calls for the MTX system:

===========================================================================
   pid = fork()  : fork a child process with an identical Umode image;
                      parent returns child's pid, child returns 0.

   r = exec(cmdline): cmdline="cmd arg1 arg2 .... argn"

   write all Umode programs as   main(int argc, char *argv[])

return -1 if the syscall fails.      
===========================================================================
