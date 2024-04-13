rfbPowM64(uint64_t b, uint64_t e, uint64_t m)
{
  uint64_t r;
  for(r=1;e>0;e>>=1)
  {
    if(e&1) r=rfbMulM64(r,b,m);
    b=rfbMulM64(b,b,m);
  }
  return r;
}