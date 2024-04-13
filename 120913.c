static unsigned encodeLZ77(uivector* out, Hash* hash,
                           const unsigned char* in, size_t inpos, size_t insize, unsigned windowsize,
                           unsigned minmatch, unsigned nicematch, unsigned lazymatching)
{
  size_t pos;
  unsigned i, error = 0;
  /*for large window lengths, assume the user wants no compression loss. Otherwise, max hash chain length speedup.*/
  unsigned maxchainlength = windowsize >= 8192 ? windowsize : windowsize / 8;
  unsigned maxlazymatch = windowsize >= 8192 ? MAX_SUPPORTED_DEFLATE_LENGTH : 64;

  unsigned usezeros = 1; /*not sure if setting it to false for windowsize < 8192 is better or worse*/
  unsigned numzeros = 0;

  unsigned offset; /*the offset represents the distance in LZ77 terminology*/
  unsigned length;
  unsigned lazy = 0;
  unsigned lazylength = 0, lazyoffset = 0;
  unsigned hashval;
  unsigned current_offset, current_length;
  unsigned prev_offset;
  const unsigned char *lastptr, *foreptr, *backptr;
  unsigned hashpos;

  if(windowsize <= 0 || windowsize > 32768) return 60; /*error: windowsize smaller/larger than allowed*/
  if((windowsize & (windowsize - 1)) != 0) return 90; /*error: must be power of two*/

  if(nicematch > MAX_SUPPORTED_DEFLATE_LENGTH) nicematch = MAX_SUPPORTED_DEFLATE_LENGTH;

  for(pos = inpos; pos < insize; pos++)
  {
    size_t wpos = pos & (windowsize - 1); /*position for in 'circular' hash buffers*/
    unsigned chainlength = 0;

    hashval = getHash(in, insize, pos);

    if(usezeros && hashval == 0)
    {
      if (numzeros == 0) numzeros = countZeros(in, insize, pos);
      else if (pos + numzeros > insize || in[pos + numzeros - 1] != 0) numzeros--;
    }
    else
    {
      numzeros = 0;
    }

    updateHashChain(hash, wpos, hashval, numzeros);

    /*the length and offset found for the current position*/
    length = 0;
    offset = 0;

    hashpos = hash->chain[wpos];

    lastptr = &in[insize < pos + MAX_SUPPORTED_DEFLATE_LENGTH ? insize : pos + MAX_SUPPORTED_DEFLATE_LENGTH];

    /*search for the longest string*/
    prev_offset = 0;
    for(;;)
    {
      if(chainlength++ >= maxchainlength) break;
      current_offset = hashpos <= wpos ? wpos - hashpos : wpos - hashpos + windowsize;

      if(current_offset < prev_offset) break; /*stop when went completely around the circular buffer*/
      prev_offset = current_offset;
      if(current_offset > 0)
      {
        /*test the next characters*/
        foreptr = &in[pos];
        backptr = &in[pos - current_offset];

        /*common case in PNGs is lots of zeros. Quickly skip over them as a speedup*/
        if(numzeros >= 3)
        {
          unsigned skip = hash->zeros[hashpos];
          if(skip > numzeros) skip = numzeros;
          backptr += skip;
          foreptr += skip;
        }

        while(foreptr != lastptr && *backptr == *foreptr) /*maximum supported length by deflate is max length*/
        {
          ++backptr;
          ++foreptr;
        }
        current_length = (unsigned)(foreptr - &in[pos]);

        if(current_length > length)
        {
          length = current_length; /*the longest length*/
          offset = current_offset; /*the offset that is related to this longest length*/
          /*jump out once a length of max length is found (speed gain). This also jumps
          out if length is MAX_SUPPORTED_DEFLATE_LENGTH*/
          if(current_length >= nicematch) break;
        }
      }

      if(hashpos == hash->chain[hashpos]) break;
      
      if(numzeros >= 3 && length > numzeros) {
        hashpos = hash->chainz[hashpos];
        if(hash->zeros[hashpos] != numzeros) break;
      } else {
        hashpos = hash->chain[hashpos];
        /*outdated hash value, happens if particular value was not encountered in whole last window*/
        if(hash->val[hashpos] != (int)hashval) break;
      }
    }

    if(lazymatching)
    {
      if(!lazy && length >= 3 && length <= maxlazymatch && length < MAX_SUPPORTED_DEFLATE_LENGTH)
      {
        lazy = 1;
        lazylength = length;
        lazyoffset = offset;
        continue; /*try the next byte*/
      }
      if(lazy)
      {
        lazy = 0;
        if(pos == 0) ERROR_BREAK(81);
        if(length > lazylength + 1)
        {
          /*push the previous character as literal*/
          if(!uivector_push_back(out, in[pos - 1])) ERROR_BREAK(83 /*alloc fail*/);
        }
        else
        {
          length = lazylength;
          offset = lazyoffset;
          hash->head[hashval] = -1; /*the same hashchain update will be done, this ensures no wrong alteration*/
          hash->headz[numzeros] = -1; /*idem*/
          pos--;
        }
      }
    }
    if(length >= 3 && offset > windowsize) ERROR_BREAK(86 /*too big (or overflown negative) offset*/);

    /*encode it as length/distance pair or literal value*/
    if(length < 3) /*only lengths of 3 or higher are supported as length/distance pair*/
    {
      if(!uivector_push_back(out, in[pos])) ERROR_BREAK(83 /*alloc fail*/);
    }
    else if(length < minmatch || (length == 3 && offset > 4096))
    {
      /*compensate for the fact that longer offsets have more extra bits, a
      length of only 3 may be not worth it then*/
      if(!uivector_push_back(out, in[pos])) ERROR_BREAK(83 /*alloc fail*/);
    }
    else
    {
      addLengthDistance(out, length, offset);
      for(i = 1; i < length; i++)
      {
        pos++;
        wpos = pos & (windowsize - 1);
        hashval = getHash(in, insize, pos);
        if(usezeros && hashval == 0)
        {
          if (numzeros == 0) numzeros = countZeros(in, insize, pos);
          else if (pos + numzeros > insize || in[pos + numzeros - 1] != 0) numzeros--;
        }
        else
        {
          numzeros = 0;
        }
        updateHashChain(hash, wpos, hashval, numzeros);
      }
    }
  } /*end of the loop through each character of input*/

  return error;
}