static long mng_get_long(unsigned char *p)
{
  return((long) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]));
}