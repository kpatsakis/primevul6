  inline int32 ByteSwapInt32ForBigEndian(int32 x) {
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return le32toh(x);
#else
    return x;
#endif
  }