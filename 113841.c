bitset_invert(BitSetRef bs)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { bs[i] = ~(bs[i]); }
}