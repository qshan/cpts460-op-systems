CptS 460 Operating Systems and Computer Architecture
======================

The code from CptS 460 at WSU with K.C. Wang. This course covered booting, process management and synchronization, simple I/O drivers, and syscalls. Code and concepts are pulled from K.C's book, Design and Implementation of the MTX Operating System. Each lab has it's own README with more information about the code:

1.  Booter - a program to load (and run) binaries from disk
2.  tswitch - manage the stack when switching processes
3.  syscalls - implement syscalls including fork (kernel mode only), wait, and process switching
4.  fork/exec - fork that creates child with same umode image and exec to run new image
5.  Midterm - pipes and exercises from the text
6.  timers/interrupts - implement an on-screen clock using interrupts
7.  semaphores/serial driver - semaphores for process synchronization and a serial driver (_connected to QEMU's virtual serial as other tty devices_)
8.  Final - a "shell" with umode applications such as cat, grep.


## A couple of notes:
  * All of these labs were run in QEMU on Linux, most include a simple script to compile and run the Lab (_look for mk or q_).
  * the vdisk (virtual disk) in each lab folder may have been corrupted at some point, I have saved a "backup" in the resources folder. (_There were multiple versions of the vdisk during the course so that file still may not work for all labs._)
  * this code hasn't been reworked or cleaned up since the class ended, there's likely bugs and errors in the code still.
## Setup

You will need to install bin86, bcc and qemu to get this code to compile and run correctly.

## Credits
* K.C. Wang provided a large amount of starter code for his assignments, then asked us to build upon it.
* I teamed up with the Daniel Johnson (@dj618) on the labs we were allowed to work in pairs (I'll try to update the relevant comment blocks).
