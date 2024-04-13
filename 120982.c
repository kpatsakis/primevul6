static unsigned char readBitFromReversedStream(size_t* bitpointer, const unsigned char* bitstream)
{
  unsigned char result = (unsigned char)((bitstream[(*bitpointer) >> 3] >> (7 - ((*bitpointer) & 0x7))) & 1);
  (*bitpointer)++;
  return result;
}