readconf_readfixed(uschar *s, int terminator)
{
int yield = 0;
int value, count;
if (!isdigit(*s)) return -1;
(void)sscanf(CS  s, "%d%n", &value, &count);
s += count;
yield = value * 1000;
if (*s == '.')
  {
  int m = 100;
  while (isdigit((*(++s))))
    {
    yield += (*s - '0') * m;
    m /= 10;
    }
  }

return (*s == terminator)? yield : (-1);
}