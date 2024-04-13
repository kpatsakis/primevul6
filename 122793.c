  for (i = 0; i < size; i++) {
    printf("%02x ", data[i]);
    if ((i + 1) % 16 == 0) printf("\n");
  }