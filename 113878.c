bitset_copy(BitSetRef dest, BitSetRef bs)
{
  int i;
  for (i = 0; i < (int )BITSET_SIZE; i++) { dest[i] = bs[i]; }
}