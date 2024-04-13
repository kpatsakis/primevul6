  for (i = 0; i < sizeof(dtr) / sizeof(WORD); i++) {
    *tmp_dst++ = SwapWord((BYTE *)tmp_src++, sizeof(WORD));
  }