static unsigned getHash(const unsigned char* data, size_t size, size_t pos)
{
  unsigned result = 0;
  if (pos + 2 < size)
  {
    /*A simple shift and xor hash is used. Since the data of PNGs is dominated
    by zeroes due to the filters, a better hash does not have a significant
    effect on speed in traversing the chain, and causes more time spend on
    calculating the hash.*/
    result ^= (unsigned)(data[pos + 0] << 0u);
    result ^= (unsigned)(data[pos + 1] << 4u);
    result ^= (unsigned)(data[pos + 2] << 8u);
  } else {
    size_t amount, i;
    if(pos >= size) return 0;
    amount = size - pos;
    for(i = 0; i < amount; i++) result ^= (unsigned)(data[pos + i] << (i * 8u));
  }
  return result & HASH_BIT_MASK;
}