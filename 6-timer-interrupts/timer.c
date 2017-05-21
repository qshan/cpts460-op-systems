/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

u16 tick;

typedef struct timerq{
  struct timeq *next;
  int time;
  struct PROC *proc;
  int action;
}TimerQEntr;


typedef struct clock{
  u16 hours;
  u16 min;
  u16 sec;

}CLOCK;

TimerQEntr tqtable[NPROC], *tq;


CLOCK master_clock;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int printTimeq()
{
  TimerQEntr *t = tq;
  PROC *prc;
  printf("%s\n", delimiter);
  while(t)
  {
    prc = t->proc;
    printf("[%d|%d]-> ", prc->pid, t->time);
    t = t->next;
  }
  printf("\n");
  printf("%s\n", delimiter);

}

int kitimer(u16 time)
{
  TimerQEntr *curr = tq, *newent, *prev = 0;
  //set the time in the tq entry
  tqtable[running->pid].time = time;
  //get a pointer for that
  newent = &(tqtable[running->pid]);
  //lock it down
  lock();
  //if the list is emty
  if(!curr)
  { //add the first timer element
    tq = &(tqtable[running->pid]);
  }
  else //we got a live one!
  {
    while(curr){//traverse q and insert newent...
      //before current node?
      if(newent->time < curr->time)
      {
        //adjust "next's" time by what we have left
        curr->time -= newent->time;
        //check if we're at head
        if(prev){ //first element?
          prev->next = newent;
        }
        else{
          tq = newent; //reset head
        }
        //assign pointers and such
        newent->next = curr;
        break;
      }
      //somewhere after this node
      else if(newent->time >= curr->time)
      {
        //decrement OUR time by next nodes time
        newent->time -= curr->time;
        //move pointers to search
        prev = curr;
        curr = curr->next;
        //hit the end of the list
        if(curr == 0)
        {
          //add newent as last element
          if(prev){prev->next = newent;}
          newent->next = curr;
        }
      }
    }
  }
  //we can put this back now
  unlock();
  //printy things to be helpful
  printTimeq();
  //go to sleep, until we wakeup this timer q entry
  ksleep(newent);
}


// write clock on bottom right in this format
// 00:00:00 (8 chars from end)
int draw_clock()
{
  int pos, w, offset, i;
  //current position

  //find absolute position (from base) to print from the current origin
  //to the last 10 chars on screen
  offset = org + 2*(24*80 - 10);
  w = color;

  //this code block is gross, don't judge me
  //print the clock and k/u for the mode we iterrupted from
  w = (w << 8) + ('0' + (master_clock.hours / 10) );
  put_word(w, base, offset);
  w = color;
  w = (w << 8) + ('0' + (master_clock.hours % 10) );
  put_word(w, base, offset+2);
  w = color;
  w = (w << 8) + (':');
  put_word(w, base, offset+4);
  w = color;
  w = (w << 8) + ('0' + (master_clock.min / 10) );
  put_word(w, base, offset+6);
  w = color;
  w = (w << 8) + ('0' + (master_clock.min % 10) );
  put_word(w, base, offset+8);
  w = color;
  w = (w << 8) + (':');
  put_word(w, base, offset+10);
  w = color;
  w = (w << 8) + ('0' + (master_clock.sec / 10) );
  put_word(w, base, offset+12);
  w = color;
  w = (w << 8) + ('0' + (master_clock.sec % 10) );
  put_word(w, base, offset+14);
  w = color;
  w = (w << 8) + ((running->inkmode > 1 ? 'K' : 'U'));
  put_word(w, base, offset+16);
}

//Used to write empty chars to the screen over char
int clear_clock()
{
  int w, offset, i;
  //find absolute position (from base) to print from the current origin
  //to the last 10 chars on screen
  offset = org + 2*(24*80 - 10);
  w = color;
  //print 9 chars to overwrite all of the clock
  for ( i = 0; i < 9; i++) {
    put_word(w, base, offset+(i*2));
  }
}



int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */
  int i;
  TimerQEntr q;
  printf("timer init\n");
  //0 out timer stuff
  tick = 0;
  master_clock.hours = master_clock.min = master_clock.sec = 0;

  //0 out, and config Timer Q stuff
  tq = 0;
  q.next = 0;
  q.time = 0;
  q.action = 0;
  for ( i = 0; i < NPROC; i++) {
    //set the proc pointer to the appropriate proc ;)
    q.proc = &(proc[i]);
    tqtable[i] = q;
  }
  //config low level
  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte
  enable_irq(TIMER_IRQ);
}

/*===================================================================*
 *		    timer interrupt handler       		      *
 *===================================================================*/
int thandler()
{
  tick++;
  tick %= 60;
  if (tick == 0){                        // at each second
      master_clock.sec++;
      master_clock.sec %= 60;
    if (master_clock.sec == 0){          // at each minute
          master_clock.min++;
          master_clock.min %= 60;
          if (master_clock.min == 0){    // at each hour
              master_clock.hours++;
              master_clock.hours %= 24;
          }
      }
      //write the clock to the screen
      draw_clock();
      //we come from user mode?
      if(running->inkmode < 2)
      {
        //decrement time
        running->time--;
        color = running->pid + 0x0A;
        //print the proc and time left
        printf("proc %d: time = %d\n", running->pid, running->time);
        //we have run out of our time slice
        if(running->time == 0)
        {
          //reset the switch time
          running->time = SWITCH_TIME;
          //re-enable interrupts
          out_byte(0x20, 0x20);
          tswitch();
          return;
        }
      }

      //Timer Q management
      if(tq) //&& running->inkmode < 2) ?
      {
        tq->time--;
        //wakeup all expired timers
        while (tq->time == 0) {
          kwakeup(tq);
          tq = tq->next;
        }
      }

  }
  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}
