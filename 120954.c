static int addBitToStream(size_t* bitpointer, ucvector* bitstream, unsigned char bit)
{
  /*add a new byte at the end*/
  if(((*bitpointer) & 7) == 0)
  {
    if (!ucvector_push_back(bitstream, (unsigned char)0)) return 83;
  }

  /*earlier bit of huffman code is in a lesser significant bit of an earlier byte*/
  (bitstream->data[bitstream->size - 1]) |= (bit << ((*bitpointer) & 0x7));
  (*bitpointer)++;

  return 0;
}