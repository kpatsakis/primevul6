DDWORD SwapDDWord(BYTE *p, int size) {
  union BYTES2DDWORD
  {
      DDWORD ddword;
      BYTE   bytes[sizeof(DDWORD)];
  };
  
  union BYTES2DDWORD converter;
  converter.ddword = 0;
  int i = 0;  
  int correct = size > sizeof(DDWORD) ? sizeof(DDWORD) : size;
  
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
  
  return converter.ddword;
}