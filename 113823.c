bitset_set_all(BitSetRef bs)
{
  int i;
  for (i = 0; i < BITSET_SIZE; i++) { bs[i] = ~((Bits )0); }
}