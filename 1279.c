static int FontCompare(const void *x,const void *y)
{
  char
    *p,
    *q;

  p=(char *) *((char **) x);
  q=(char *) *((char **) y);
  while ((*p != '\0') && (*q != '\0') && (*p == *q))
  {
    p++;
    q++;
  }
  return(*p-(*q));
}