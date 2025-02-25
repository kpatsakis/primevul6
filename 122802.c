char *to_utf8(size_t len, char *buf) {
  int i, j = 0;
  /* worst case length */
  if (len > 10000) {	// deal with this by adding an arbitrary limit
     printf("suspecting a corrupt file in UTF8 conversion\n");
     exit(-1);
  }
  char *utf8 = malloc(3 * len / 2 + 1);

  for (i = 0; i < len - 1; i += 2) {
    unsigned int c = SwapWord((BYTE *)buf + i, 2);
    if (c <= 0x007f) {
      utf8[j++] = 0x00 | ((c & 0x007f) >> 0);
    } else if (c < 0x07ff) {
      utf8[j++] = 0xc0 | ((c & 0x07c0) >> 6);
      utf8[j++] = 0x80 | ((c & 0x003f) >> 0);
    } else {
      utf8[j++] = 0xe0 | ((c & 0xf000) >> 12);
      utf8[j++] = 0x80 | ((c & 0x0fc0) >> 6);
      utf8[j++] = 0x80 | ((c & 0x003f) >> 0);
    }
  }

  /* just in case the original was not null terminated */
  utf8[j++] = '\0';

  return utf8;
}