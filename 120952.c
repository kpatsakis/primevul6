static unsigned generateFixedLitLenTree(HuffmanTree* tree)
{
  unsigned i, error = 0;
  unsigned* bitlen = (unsigned*)calloc(NUM_DEFLATE_CODE_SYMBOLS, sizeof(unsigned));
  if(!bitlen) return 83; /*alloc fail*/

  /*288 possible codes: 0-255=literals, 256=endcode, 257-285=lengthcodes, 286-287=unused*/
  for(i =   0; i <= 143; i++) bitlen[i] = 8;
  for(i = 144; i <= 255; i++) bitlen[i] = 9;
  for(i = 256; i <= 279; i++) bitlen[i] = 7;
  for(i = 280; i <= 287; i++) bitlen[i] = 8;

  error = HuffmanTree_makeFromLengths(tree, bitlen, NUM_DEFLATE_CODE_SYMBOLS, 15);

  free(bitlen);
  return error;
}