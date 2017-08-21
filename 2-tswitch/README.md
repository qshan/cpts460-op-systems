tswitch and
======================

### Lab Assignment 2

```
Chapter 4
```

This lab was to demonstrate switching processes from within the kernel. tswitch() is an assembly function that in essence dumps the state of a process (the register values and such) to the stack, then proceeds to load a waiting process and it's state from that processes stack then resume executing.
In this lab we had to develop a priority queue to hold process structs as well as the code to resume from switching.


#### Print Partition Info
To print the entries within the partition table first we load the MBR to a buffer, then we offset into that block by 446 bytes (or 0x1BE). From here we should have 4 partition structs detailing the partitions on the disk, we can print and increment our printer to read through them (at this point we wont worry about extended partitions).
