static void addPaddingBits(unsigned char* out, const unsigned char* in,
                           size_t olinebits, size_t ilinebits, unsigned h)
{
  /*The opposite of the removePaddingBits function
  olinebits must be >= ilinebits*/
  unsigned y;
  size_t diff = olinebits - ilinebits;
  size_t obp = 0, ibp = 0; /*bit pointers*/
  for(y = 0; y < h; y++)
  {
    size_t x;
    for(x = 0; x < ilinebits; x++)
    {
      unsigned char bit = readBitFromReversedStream(&ibp, in);
      setBitOfReversedStream(&obp, out, bit);
    }
    /*obp += diff; --> no, fill in some value in the padding bits too, to avoid
    "Use of uninitialised value of size ###" warning from valgrind*/
    for(x = 0; x < diff; x++) setBitOfReversedStream(&obp, out, 0);
  }
}