WORD SwapWord(BYTE *p, int size) {
  union BYTES2WORD
  {
      WORD word;
      BYTE bytes[sizeof(WORD)];
  };
  
  union BYTES2WORD converter;  
  converter.word = 0;
  int i = 0;
  int correct = size > sizeof(WORD) ? sizeof(WORD) : size;

#ifdef WORDS_BIGENDIAN
  for (i = 0; i < correct; ++i)
  {
      converter.bytes[i] = p[correct - i];
  }
#else
  for (i = 0; i < correct; ++i)
  {
      converter.bytes[i] = p[i];
  }
#endif
  
  return converter.word;
}