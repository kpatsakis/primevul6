static String php_base64_decode(const char *str, int length, bool strict) {
  const unsigned char *current = (unsigned char*)str;
  int ch, i = 0, j = 0, k;
  /* this sucks for threaded environments */

  String retString(length, ReserveString);
  unsigned char* result = (unsigned char*)retString.bufferSlice().ptr;

  /* run through the whole string, converting as we go */
  while ((ch = *current++) != '\0' && length-- > 0) {
    if (ch == base64_pad) {
      if (*current != '=' && ((i % 4) == 1 || (strict && length > 0))) {
        if ((i % 4) != 1) {
          while (isspace(*(++current))) {
            continue;
          }
          if (*current == '\0') {
            continue;
          }
        }
        return String();
      }
      continue;
    }

    ch = base64_reverse_table[ch];
    if ((!strict && ch < 0) || ch == -1) {
      /* a space or some other separator character, we simply skip over */
      continue;
    } else if (ch == -2) {
      return String();
    }

    switch(i % 4) {
    case 0:
      result[j] = ch << 2;
      break;
    case 1:
      result[j++] |= ch >> 4;
      result[j] = (ch & 0x0f) << 4;
      break;
    case 2:
      result[j++] |= ch >>2;
      result[j] = (ch & 0x03) << 6;
      break;
    case 3:
      result[j++] |= ch;
      break;
    }
    i++;
  }

  k = j;
  /* mop things up if we ended on a boundary */
  if (ch == base64_pad) {
    switch(i % 4) {
    case 1:
      return String();
    case 2:
      k++;
    case 3:
      result[k] = 0;
    }
  }
  retString.setSize(j);
  return retString;
}