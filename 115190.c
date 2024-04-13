static MngPair mng_read_pair(MngPair previous_pair,int delta_type,
  unsigned char *p)
{
  MngPair
    pair;
  /*
    Read two ssize_ts from CLON, MOVE or PAST chunk
  */
  pair.a=(long) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
  pair.b=(long) ((p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7]);

  if (delta_type != 0)
    {
      pair.a+=previous_pair.a;
      pair.b+=previous_pair.b;
    }

  return(pair);
}