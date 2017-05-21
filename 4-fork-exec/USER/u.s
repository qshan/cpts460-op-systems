.globl _main0,_syscall,_exit,_getcs,auto_start
.globl _getc, _putc, _color

auto_start:
call _main0

! if main() ever return, exit(0)
 push  #0
call  _exit

_syscall:
int    80
ret



_getcs:
mov   ax, cs
ret



!---------------------------------------------
!  char getc( ) = syscall(7, 0,0,0)
!---------------------------------------------

!----------------------------------------------
! int putc(char c) = syscall(8, char c, 0,0)
!----------------------------------------------
