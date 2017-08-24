A Booter program
======================

## Lab Assignment 1

```
Chapter 2: 2.1.4, 2.1.5, 2.3.4, 2.4, 2.7, 2.8, 2.9
Chapter 3: 3.3
```

The first lab assignment was to create a booter program that would load a binary from a virtual disk into memory and boot it. We were provided a disk image, an assembly file [bs.s](start/bs.s), and a C file [bc.c](start/bc.c) to start and asked to complete the following tasks:

1. make a shell script to compile, assemble, and link the .s and .c files to an executable and then put that executable in the virtual disk

2. Write gets() and printf() in C using the provided getc()/putc() functions.

3. Print out partition information stored in the Master Boot Recor (MBR) of the virtual disk

This program will ask for a users name and repeat it back to demonstrate our gets/printf is working as intended.


##### Shell script
The shell script uses as86, bcc, then ld86 to compile a.out from the source files. Then we use dd to copy our executable to the vdisk Master Boot Record (first 512 bytes on the disk). Finally we call QEMU to launch a VM to boot our vdisk.

``` bash
as86 -o bs.o  bs.s
bcc -c -ansi  bc.c bio.c
ld86 -d bs.o bc.o bio.o /usr/lib/bcc/libc.a

dd if=a.out of=vdisk bs=512 seek=1  conv=notrunc

qemu-system-x86_64 -hda vdisk

```

##### gets()/prints()
When the booter runs there is no operating systemavailable to provide standard library functions like gets() or printf(), we've got the bios functionality to work with. Given a getc() and putc() in assembly like so:

``` mipsasm
    !------------------------------------------------------
    !  char getc( )   function: return a char from keyboard
    !------------------------------------------------------
_getc:
    xorb   ah,ah           ! clear ah
    int    0x16            ! call BIOS to get a char in AX
    andb   al,#0x7F        ! 7-bit ascii
    ret

    !---------------------------------------------------
    ! int putc(char c)  function: print a char to screen
    !---------------------------------------------------
_putc:
    push   bp
    mov    bp,sp

    movb   al,4[bp]        ! get the char into aL
    movb   ah,#14          ! aH = 14
    mov    bx,_color       ! cyan
    int    0x10            ! call BIOS to display the char

    pop    bp
    ret
```
We can define prints()/gets() without too much hassle. Remember that our parameters are pushed onto the stack in reverse order, thus when we enter printf() the first parameter is the format string, immediately followed on the stack by the other parameters. So in our printf we grab the address of the format string and then offset that address to the next parameters, from there we can keep incrementing this pointer to get to the next parameter for printf. Like so:

``` C
...
int *inptr = &fmt + 1;
char *cp = fmt;
...
```

#### Print Partition Info
To print the entries within the partition table first we load the MBR to a buffer, then we offset into that block by 446 bytes (or 0x1BE). From here we should have 4 partition structs detailing the partitions on the disk, we can print and increment our printer to read through them (at this point we wont worry about extended partitions).

``` C
  p = mbr + 0x1BE;
  for(i = 0; i < 4; ++i, p++)
  {
    printf("drive: %u\n", p->drive);
    printf("type: %u\n", p->type);
    printf("start sect: %u\n",p->start_sector);
    printf("sector count: %u\n", p->nr_sectors);
  }
```