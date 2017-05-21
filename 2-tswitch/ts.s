!********************************************************************
!Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
!This program is free software: you can redistribute it and/or modify
!it under the terms of the GNU General Public License as published by
!the Free Software Foundation, either version 3 of the License, or
!(at your option) any later version.

!This program is distributed in the hope that it will be useful,
!but WITHOUT ANY WARRANTY; without even the implied warranty of
!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!GNU General Public License for more details.

!You should have received a copy of the GNU General Public License
!along with this program.  If not, see <http://www.gnu.org/licenses/>.
!********************************************************************/

!mov DEST, SOURCE

        OSSEG  = 0x1000

       .globl _main,_running,_scheduler
       .globl _proc, _procSize
       .globl _tswitch,_resume

        jmpi   start,OSSEG

start:	mov  ax,cs           !set ds = ss = es = cs
	mov  ds,ax
	mov  ss,ax
  mov  es,ax
	mov  sp,#_proc             !establish stack pointer
	add  sp,_procSize

	call _main

_tswitch:
SAVE:
	push ax                    !save all registers to stack
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	pushf                      !push flags (2 bytes)

  !ds = ss = 0x1000
  push ds
  push ss

  ! set the stack pointer in _running struct
	mov  bx,_running
	mov  2[bx],sp


FIND:	call _scheduler   !SWITCH

_resume:
RESUME:
	mov  bx,_running         ! re-establish stack pointer from *ksp (_running)
	mov  sp,2[bx]

  !restoring these to ss/ds cause the program to begin again at main
  pop ss
  !mov ss,ax
  pop ds
  !mov ds,ax
	popf                     !pop flags
	pop  di                  ! load all registers from stack (rev order)
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax

	ret
