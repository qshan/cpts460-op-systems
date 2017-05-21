A Booter program
======================



                  CS460 Lab Assignment #1
###########################################################################
                    DUE : to be posted
###########################################################################

1. REQURIEMENT:
   To deveop a booter program to boot up the MTX operating system kernel from a
   (virtual) hard disk partition.

2. Background: Reading list:
               Chapter 2: 2.1.4, 2.1.5, 2.3.4, 2.4, 2.7, 2.8, 2.9
               Chapter 3: 3.3              

3. DEMO disk images:

3-1. Virtual FD disk image:
     Download samples/FDimage, and use it as a virtual FD disk, as in
              qemu -fda -no-fd-bootchk
     MTX should boot up.


3-2. Virtual HD disk image:
     Download samples/vdisk, and use it as a virtual HD disk, as in
              qemu -hda vdisk
     When the partition memu shows up, boot from a partition number.

============================================================================
   LAB#1 IS FOR YOU TO WRITE A BOOTER PROGRAM TO REPLACE the booter on the
   HD to boot up a MTX kernel form a HD partition.
============================================================================

4. PRE-WORK

PART 1: DO THIS but no need to turn in.

Page 53: a mk sh script for assemble-compile-link .s and .c files into a booter
         and dump it to the MBR of a FD image

Page 54: bs.s code: COPY & PASTE to a bs.s file
                    STUDY bs.s code carefully.

Page 64: bc.c code: COPY & PASTE to a bc.c file (ext2.h is in samples/LAB2.PRE)
                    STUDY bc.c code carefully.

DO: With YOUR bs.s, bc.c and FDimage,
    Run       mk
    Then, boot FDimage under QEMU.
============================================================================

            PART 2: REQUIRED   DUE: in one week

4-1. Given: the following bs.s file.
     NOTE:  This ts.s is for booting from HD, NOT FD.

!---------------------- bs.s file -------------------------------------------
         BOOTSEG  = 0x9000
	 SSP      = 32*1024

        .globl _main, _prints,_color,_dap               ! IMPORT
        .globl _diskr,_getc,_putc,_setds,_error         ! EXPORT

        !-------------------------------------------------------------------
        ! MBR loaded at segment 0x07C0. Load entire booter to segment 0x9000
        !-------------------------------------------------------------------
        mov  ax,#BOOTSEG
        mov  es,ax
        xor  bx,bx          ! clear BX = 0
        !---------------------------------------------------
        !  read entire booter in sector#1-8KB to 0x9000
        !---------------------------------------------------
        mov  dx,#0x0080     ! dh=head=0, dL=0x80=HD or USB's MBR
        xor  cx,cx
        incb cl             ! cyl 0, sector 1
	incb cl             ! cyl 0, sector 2
        mov  ax, #0x0220    ! READ 16 sectors (booter<8KB)
        int  0x13

        jmpi next,BOOTSEG   ! CS=BOOTSEG, IP=next

next:
        mov  ax,cs          ! establish segments again
        mov  ds,ax          ! we know ES,CS=0x9000. Let DS=CS  
        mov  ss,ax
        mov  es,ax
        mov  sp,#SSP        ! 32 KB stack

!----------------------------------------------------------------------------
!             BOOTER EXECUTION IMAGE in memory
!                
!   0x1000                           0x9000                        0xA000  
!   ---------------------------------|---------------|-------------|----------
!   |   |//|                         |Code|Data|stack|             |  ROM
!   ----|----------------------------|---------------|-------------|----------
!       0x7C00                       CS              sp=32KB
!                                    DS              from SS
!                                    SS
!---------------------------------------------------------------------------
        mov  ax,#0x0012     ! Call BIOS for 640x480 color mode     
	int  0x10
!------------------------------------------------------------
        call _main          ! call main() in C

        test ax, ax         ! check return value from main()
        je   _error         ! main() return 0 if error

        jmpi 0, 0x1000      ! FAR jump to (segment, offset)=(0x1000, 0)

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


!setds(segment) : set DS to segment to R/W memory outside BOOTSEG
_setds:
	push  bp
	mov   bp,sp
	mov   ax,4[bp]
	mov   ds,ax
	pop   bp
	ret

! diskr(): read disk sectors specified by _dap in C code		
_diskr:
         mov dx, #0x0080
	 mov ax, #0x4200
	 mov si, #_dap

         int 0x13               ! call BIOS to read the block
         jb  _error             ! to error() if CarryBit is on [read failed]
	 ret

        !------------------------------
        !       error() & reboot
        !------------------------------
_error:
        push #msg
        call _prints
        int  0x19               ! reboot
msg:   .asciz  "\n\rError!\n\r"


5-2. The following bc.c file

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define  GREEN  10         // color byte for putc()
#define  CYAN   11
#define  RED    12
#define  MAG    13
#define  YELLOW 14

struct partition {         // Partition table entry in MBR
       u8  drive;          // 0x80 - active
       u8  head;	   // starting head
       u8  sector;	   // starting sector
       u8  cylinder;       // starting cylinder
       u8  type;	   // partition type
       u8  end_head;       // end head
       u8  end_sector;	   // end sector
       u8  end_cylinder;   // end cylinder
       u32 start_sector;   // starting sector counting from 0
       u32 nr_sectors;     // nr of sectors in partition
};

struct dap{                // DAP for extended INT 13-42  
       u8   len;           // dap length=0x10 (16 bytes)
       u8   zero;          // must be 0  
       u16  nsector;       // number of sectors to read: 1 to 127
       u16  addr;          // memory address = (segment:addr)
       u16  segment;    
       u32  s1;            // low  4 bytes of sector#
       u32  s2;            // high 4 bytes of sector#
};

struct dap dap, *dp;       // global dap struct
u16 color = RED;           // initial color for putc()

#define  BOOTSEG 0x9000

// #include "bio.c" <========= WRITE YOUR OWN io.c file !!!!

char mbr[512];
char ans[64];

// load a disk sector to (DS, addr), where addr is an offset in segment
int getSector(u32 sector, char *addr)
{
  dp->addr    = addr;
  dp->s1      = (u32)sector;
  diskr();    // call int13-43 in assembly
}

int main()
{
  int i;
  struct partition *p;
  printf("booter start in main()\n");

  // initialize the dap struct
  dp = &dap;
  dp->len  = 0x10;        // must be 16
  dp->zero = 0;           // must be 0
  dp->nsector = 1;        // load one sector
  dp->addr = 0;           // will set to addr              
  dp->segment = BOOTSEG;  // load to which segment
  dp->s1 = 0;             // will set to LBA sector#
  dp->s2 = 0;             // high 4-byte address s2=0

  getSector((u32)0, (u16)mbr); // get MBR
  ==========================================================
  In the MBR, partition table begins at byte offset 0x1BE
  // DO #1:  WRITE C CODE TO PRINT THE PARTITIONS
  ==========================================================
  color = CYAN;
  while(1){
    printf("what's your name? ");
    gets(ans);
    if (strcmp(ans, "quit")==0){
      printf("\nexit main()\n");
      break;
    }
    printf("\nWelcome %s!\n", ans);
  }
}
====================================================================
DO #2: The bs.s file contains
          char getc()         : return a char from keyboard
          int  putc(char c)   : display a char to screen  
   which are supported by BIOS. There are NO OTHER terminal I/O functions!!!

   Write your OWN gets(char s[ ]) fucntion to get a string.
   Write your OWN printf(char *fmt, ...) as in CS360

Then, include YOUR io.c file in bc.c.
      Do NOT use KCW's io.o in linking step.
===================================================================

5-3. Use BCC to generate a one-segment binary executable a.out WITHOUT header

     as86 -o bs.o  bs.s
     bcc -c -ansi  bc.c
     ld86 -d bs.o bc.o YOUR_io.o /usr/lib/bcc/libc.a

5-4. dump a.out to a VIRTUAL HD:

     dd if=a.out of=vdisk bs=16 count=27 conv=notrunc
     dd if=a.out of=vdisk bs=512 seek=1  conv=notrunc


5-5. Run QEMU on vdisk

     qemu -hda vdisk

=======================================================================