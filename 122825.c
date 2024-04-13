DWORD SwapDWord(BYTE *p, int size) {
  union BYTES2DWORD
  {
      DWORD dword;
      BYTE  bytes[sizeof(DWORD)];
  };
  
  union BYTES2DWORD converter;
  converter.dword = 0;
  int i = 0;  
  int correct = size > sizeof(DWORD) ? sizeof(DWORD) : size;
  
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
  
  return converter.dword;
}