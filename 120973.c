static unsigned huffmanDecodeSymbol(const unsigned char* in, size_t* bp,
                                    const HuffmanTree* codetree, size_t inbitlength)
{
  unsigned treepos = 0, ct;
  if (!codetree || !codetree->tree2d)
    return 0;

  for(;;)
  {
    if(*bp >= inbitlength) return (unsigned)(-1); /*error: end of input memory reached without endcode*/
    /*
    decode the symbol from the tree. The "readBitFromStream" code is inlined in
    the expression below because this is the biggest bottleneck while decoding
    */
    ct = codetree->tree2d[(treepos << 1) + READBIT(*bp, in)];
    (*bp)++;
    if(ct < codetree->numcodes) return ct; /*the symbol is decoded, return it*/
    else treepos = ct - codetree->numcodes; /*symbol not yet decoded, instead move tree position*/

    if(treepos >= codetree->numcodes) return (unsigned)(-1); /*error: it appeared outside the codetree*/
  }
}