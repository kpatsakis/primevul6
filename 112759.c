void string_charmask(const char *sinput, int len, char *mask) {
  const unsigned char *input = (unsigned char *)sinput;
  const unsigned char *end;
  unsigned char c;

  memset(mask, 0, 256);
  for (end = input+len; input < end; input++) {
    c=*input;
    if ((input+3 < end) && input[1] == '.' && input[2] == '.'
        && input[3] >= c) {
      memset(mask+c, 1, input[3] - c + 1);
      input+=3;
    } else if ((input+1 < end) && input[0] == '.' && input[1] == '.') {
      /* Error, try to be as helpful as possible:
         (a range ending/starting with '.' won't be captured here) */
      if (end-len >= input) { /* there was no 'left' char */
        throw_invalid_argument
          ("charlist: Invalid '..'-range, missing left of '..'");
        continue;
      }
      if (input+2 >= end) { /* there is no 'right' char */
        throw_invalid_argument
          ("charlist: Invalid '..'-range, missing right of '..'");
        continue;
      }
      if (input[-1] > input[2]) { /* wrong order */
        throw_invalid_argument
          ("charlist: '..'-range needs to be incrementing");
        continue;
      }
      /* FIXME: better error (a..b..c is the only left possibility?) */
      throw_invalid_argument("charlist: Invalid '..'-range");
      continue;
    } else {
      mask[c]=1;
    }
  }
}