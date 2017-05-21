the tswitch function
======================


460 Lab Assignment #2 : MTX System
DUE: Week of 9-12-2016 : DEMO to TA in that week


1. GIVEN: MTX4.4 and MTX4.5 code in Chapter 4 of the TEXT

               2. REQUIREMENTS:

2-1. Change tswitch() code (in ts.s) as specified in Problem 4.2:

         _tswitch:
         SAVE: push ax,bx,cx,dx,bp,si,di
               pushf

         ! ADD these lines, which saves CPU's SEGMENT registers
               push ds
               push ss
         ! END of added lines

               mov  bx, _running
               mov  2[bx], _running
         FIND: call scheduler

         RESUME:

DO THIS: Write your OWN RESUME code for tswitch()

2-2. Use YOUR modified tswitch() function for the program MTX4.5,
     Run the modified MTX4.5 but without the |t|c|z|a|p| commands,
     just the |s|f|w|q| commands
