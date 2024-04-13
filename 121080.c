static unsigned readBitsFromReversedStream(size_t* bitpointer, const unsigned char* bitstream, size_t nbits)
{
  unsigned result = 0;
  size_t i;
  for(i = nbits - 1; i < nbits; i--)
  {
    result += (unsigned)readBitFromReversedStream(bitpointer, bitstream) << i;
  }
  return result;
}