static void setBitOfReversedStream0(size_t* bitpointer, unsigned char* bitstream, unsigned char bit)
{
  /*the current bit in bitstream must be 0 for this to work*/
  if(bit)
  {
    /*earlier bit of huffman code is in a lesser significant bit of an earlier byte*/
    bitstream[(*bitpointer) >> 3] |= (bit << (7 - ((*bitpointer) & 0x7)));
  }
  (*bitpointer)++;
}