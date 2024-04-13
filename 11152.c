readconf_readtime(uschar *s, int terminator, BOOL return_msec)
{
int yield = 0;
for (;;)
  {
  int value, count;
  double fraction;

  if (!isdigit(*s)) return -1;
  (void)sscanf(CS s, "%d%n", &value, &count);
  s += count;

  switch (*s)
    {
    case 'w': value *= 7;
    case 'd': value *= 24;
    case 'h': value *= 60;
    case 'm': value *= 60;
    case 's': s++;
    break;

    case '.':
    if (!return_msec) return -1;
    (void)sscanf(CS s, "%lf%n", &fraction, &count);
    s += count;
    if (*s++ != 's') return -1;
    yield += (int)(fraction * 1000.0);
    break;

    default: return -1;
    }

  if (return_msec) value *= 1000;
  yield += value;
  if (*s == terminator) return yield;
  }
/* Control never reaches here. */
}