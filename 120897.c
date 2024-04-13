static unsigned deflateNoCompression(ucvector* out, const unsigned char* data, size_t datasize)
{
  /*non compressed deflate block data: 1 bit BFINAL,2 bits BTYPE,(5 bits): it jumps to start of next byte,
  2 bytes LEN, 2 bytes NLEN, LEN bytes literal DATA*/

  size_t i, j, numdeflateblocks = (datasize + 65534) / 65535;
  unsigned datapos = 0;
  for(i = 0; i < numdeflateblocks; i++)
  {
    unsigned BFINAL, BTYPE, LEN, NLEN;
    unsigned char firstbyte;

    BFINAL = (i == numdeflateblocks - 1);
    BTYPE = 0;

    firstbyte = (unsigned char)(BFINAL + ((BTYPE & 1) << 1) + ((BTYPE & 2) << 1));
    if (!ucvector_push_back(out, firstbyte)) return 83;

    LEN = 65535;
    if(datasize - datapos < 65535) LEN = (unsigned)datasize - datapos;
    NLEN = 65535 - LEN;

    if (!ucvector_push_back(out, (unsigned char)(LEN % 256))) return 83;
    if (!ucvector_push_back(out, (unsigned char)(LEN / 256))) return 83;
    if (!ucvector_push_back(out, (unsigned char)(NLEN % 256))) return 83;
    if (!ucvector_push_back(out, (unsigned char)(NLEN / 256))) return 83;

    /*Decompressed data*/
    for(j = 0; j < 65535 && datapos < datasize; j++)
    {
      if (!ucvector_push_back(out, data[datapos++])) return 83;
    }
  }

  return 0;
}