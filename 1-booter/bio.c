
int BASE = 10;
char *table = "0123456789ABCDEF";

int rpu(unsigned int x)
{
  char c;
  if (x){
     c = (char)((x % 10) + '0');
     rpu(x / BASE);
     putc(c);
  }
}

int rpul(unsigned long x)
{
  char c;
  if (x){
     c = (char)((x % 10) + '0');
     rpul((unsigned long)(x / 10));
     putc(c);
  }
}

int printu(unsigned int x)
{
  if (x==0)
     putc('0');
  else
     rpu(x);
  putc(' ');
}

int printl(unsigned long x)
{
  if (x==0)
     putc('0');
  else
     rpul(x);
  putc(' ');

}

int printd(int x)
{
	if (x==0)
		putc('0');
	else if (x < 0)
	{
		x = -x;
		putc('-');
	}
	rpu(x);
}

int printo(unsigned int x)
{
	BASE = 8;
	printu(x);
	BASE = 10;
}

int printx(unsigned int x)
{
	BASE = 16;
	printu(x);
	BASE = 10;
}

int prints(char *str)
{

	while (*str)
	{
    if(*str == '\n')
    {
      putc('\n');
      putc('\r');
    }
    else
    {
      putc(*str);
    }
		str++;
	}
}

int printf(char *fmt, ...)
{
	int *inptr = &fmt + 1;
	char *cp = fmt;

	while(*cp)
	{
		if (*cp == '%')
			switch (*(++cp)) {
        case 'u':
					printu(*inptr++);
					break;
        case 'l':
					printl(*inptr++);
					break;
				case 'd':
					printd(*inptr++);
					break;
				case 'o':
					printo(*inptr++);
					break;
				case 'x':
					printx(*inptr++);
					break;
				case 's':
					prints(*inptr++);
					break;
				case 'c':
					putc(*inptr++);
					break;
			}
		else
		{
      if(*cp == '\n')
      {
        putc('\n');
        putc('\r');
      }
      else
      {
        putc(*cp);
      }
		}
		cp++;
	}
}


int gets(char c[])
{
  char ch = 0;
  while(ch != '\r')
  {
    ch = getc();
    putc(ch);
    *c = ch;
    c++;
  }
  *c = 0;
}

int strcmp(char *str1, char *str2)
{
  int diff = 0;

  while(*str1 && *str2)
  {
    diff = *str1 - *str2;
    if (diff != 0)
    {
      return diff;
    }
    str1++;
    str2++;
  }
  return diff;
}
