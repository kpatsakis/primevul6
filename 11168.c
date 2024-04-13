find_option(uschar *name, optionlist *ol, int last)
{
int first = 0;
while (last > first)
  {
  int middle = (first + last)/2;
  int c = Ustrcmp(name, ol[middle].name);
  if (c == 0) return ol + middle;
    else if (c > 0) first = middle + 1;
      else last = middle;
  }
return NULL;
}