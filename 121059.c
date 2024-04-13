static unsigned generateFixedDistanceTree(HuffmanTree* tree)
{
  unsigned i, error = 0;
  unsigned* bitlen = (unsigned*)malloc(NUM_DISTANCE_SYMBOLS * sizeof(unsigned));
  if(!bitlen) return 83; /*alloc fail*/

  /*there are 32 distance codes, but 30-31 are unused*/
  for(i = 0; i < NUM_DISTANCE_SYMBOLS; i++) bitlen[i] = 5;
  error = HuffmanTree_makeFromLengths(tree, bitlen, NUM_DISTANCE_SYMBOLS, 15);

  free(bitlen);
  return error;
}