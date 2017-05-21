/********************************************************************
Code base from KCW "MTX" book
/*****************************************************************/
#define VDC_INDEX      0x3D4
#define VDC_DATA       0x3D5
#define CUR_SIZE          10	/* cursor size register */
#define VID_ORG           12	/* start address register */
#define CURSOR            14	/* cursor position register */
#define LINE_WIDTH        80	/* # characters on a line */
#define SCR_LINES         25	/* # lines on the screen */
#define SCR_BYTES	      4000	/* bytes of ONE screen=25*80 */
#define CURSOR_SHAPE      15    /* block cursor for MDA/HGC/CGA/EGA/VGA... */
// attribute byte: 0x0HRGB, H=highLight; RGB determine color
#define HGREEN          0x0A
#define HCYAN           0x0B
#define HRED            0x0C
#define HPURPLE         0x0D
#define HYELLOW         0x0E
u16 base     = 0xB800;    // VRAM base address
u16 vid_mask = 0x3FFF;    // mask=Video RAM size - 1
u16 offset;               // offset from VRAM base
// extern int color;                // atttirbute byte
u16 org;                  // current display origin r.e.VRAM base
u16 row, column;          // logical row, col position

//for ease
draw_clock();
clear_clock();

// vid_init() initializes the display org=0 (row,col)=(0,0)
int vid_init()
{
  int i, w;
  org = row = column = 0;
  color = HCYAN;

  set_VDC(CUR_SIZE, CURSOR_SHAPE);   // set cursor size
  set_VDC(VID_ORG, 0);               // display origin to 0
  set_VDC(CURSOR, 0);	             // set cursor position to 0

  // clear screen
  w = 0x0700;    // White, blank char // attribute byte=0000 0111=0000 0RGB
  for (i=0; i<25*80; i++){
    put_word(w, base, 0+2*i);         // write 24*80 blanks to VGA memory
  }
}

//scroll(): scroll UP one line
int scroll()
{
  int i;
  u16 w, bytes;

  //remove clock from display
  clear_clock();

  // Normal scrolling using the 6845 registers.
  // try offset = org + ONE screen + ONE more line
  offset = org + SCR_BYTES + 2*LINE_WIDTH;

  if (offset <= vid_mask){   // offset still within vram area
    org += 2*LINE_WIDTH;     // just advance org by ONE line
  }
  else{  // offset exceeds vram area ==> reset to vram beginning by
         // copy current rows 1-24 to BASE, then reset org to 0
      for (i=0; i<24*80; i++){
      w = get_word(base, org+160+2*i);
      put_word(w, base, 0+2*i);
    }
    org = 0;
  }

  // org has been set up correctly
  offset = org + 2*24*80;   // offset = beginning of row 24
  // copy a line of BLANKs to row 24
  w = 0x0C00;  // HRGB=1100 ==> HighLight RED, Null char

  for (i=0; i<80; i++){
    put_word(w, base, offset + 2*i);
  }
  //drawk the clock again after adjusting the screen
  draw_clock();
  set_VDC(VID_ORG, org >> 1);	  /// 6845 thinks in words
}

//putc
int putc(char c)
{
  // display c to cursor position, advance cursor
  int pos, w, offset;

  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_VDC(CURSOR, offset >> 1);
    return;
  }

  if (c=='\r'){
    column=0;
    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_VDC(CURSOR, offset >> 1);
    return;
  }

  if (c=='\b'){
    if (column > 0){
      column--;

      pos = 2*(row*80 + column);
      offset = (org + pos) & vid_mask;
      put_word(0x0700, base, offset);

      set_VDC(CURSOR, offset >> 1);
    }
    return;
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  w = color;
  //w = color + (running->pid);
  w = (w << 8) + c;
  put_word(w, base, offset);
  column++;

  if (column >= 80){
    column = 0;
    row++;

    if (row>=25){
      row = 24;
      scroll();
    }
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  set_VDC(CURSOR, offset >> 1);
}

 //set_6845
int set_VDC(u16 reg, u16 val)
{
/* Set a register pair inside the 6845.
 * Registers 10-11 control the format of the cursor (how high it is, etc).
 * Registers 12-13 tell the 6845 where in video ram to start (in WORDS)
 * Registers 14-15 tell the 6845 where to put the cursor (in WORDS)
 *
 * Note that registers 12-15 work in words, i.e. 0x0000 is the top left
 * character, but 0x0001 (not 0x0002) is the next character.  This addressing
 * is different from the way the 8088 addresses the video ram, where 0x0002
 * is the address of the next character.
 */
  lock();			/* try to stop h/w loading in-between value */
  out_byte(VDC_INDEX, reg);	/* set the index register */
  out_byte(VDC_DATA,  (val>>8) & 0xFF);	/* output high byte */
  out_byte(VDC_INDEX, reg + 1);	/* again */
  out_byte(VDC_DATA,  val&0xFF);	/* output low byte */
  unlock();
}
