rfbMulM64(uint64_t x, uint64_t y, uint64_t m)
{
  uint64_t r;
  for(r=0;x>0;x>>=1)
  {
    if (x&1) r=rfbAddM64(r,y,m);
    y=rfbAddM64(y,y,m);
  }
  return r;
}