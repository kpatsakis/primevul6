  while ((d - (BYTE*)data) < size) {
    SIZECHECK(sizeof(WORD));
    name_length = SwapWord((BYTE*)d, sizeof(WORD));
    d += sizeof(WORD);
    if (TNEF->Debug >= 1)
      printf("Sent For : %s", d);
    d += name_length;

    SIZECHECK(sizeof(WORD));
    addr_length = SwapWord((BYTE*)d, sizeof(WORD));
    d += sizeof(WORD);
    if (TNEF->Debug >= 1)
      printf("<%s>\n", d);
    d += addr_length;
  }