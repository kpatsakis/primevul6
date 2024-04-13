String string_quoted_printable_decode(const char *input, int len, bool is_q) {
  assert(input);
  if (len == 0) {
    return String();
  }

  int i = 0, j = 0, k;
  const char *str_in = input;
  String ret(len, ReserveString);
  char *str_out = ret.bufferSlice().ptr;
  while (i < len && str_in[i]) {
    switch (str_in[i]) {
    case '=':
      if (i + 2 < len && str_in[i + 1] && str_in[i + 2] &&
          isxdigit((int) str_in[i + 1]) && isxdigit((int) str_in[i + 2]))
        {
          str_out[j++] = (string_hex2int((int) str_in[i + 1]) << 4)
            + string_hex2int((int) str_in[i + 2]);
          i += 3;
        } else  /* check for soft line break according to RFC 2045*/ {
        k = 1;
        while (str_in[i + k] &&
               ((str_in[i + k] == 32) || (str_in[i + k] == 9))) {
          /* Possibly, skip spaces/tabs at the end of line */
          k++;
        }
        if (!str_in[i + k]) {
          /* End of line reached */
          i += k;
        }
        else if ((str_in[i + k] == 13) && (str_in[i + k + 1] == 10)) {
          /* CRLF */
          i += k + 2;
        }
        else if ((str_in[i + k] == 13) || (str_in[i + k] == 10)) {
          /* CR or LF */
          i += k + 1;
        }
        else {
          str_out[j++] = str_in[i++];
        }
      }
      break;
    case '_':
      if (is_q) {
        str_out[j++] = ' ';
        i++;
      } else {
        str_out[j++] = str_in[i++];
      }
      break;
    default:
      str_out[j++] = str_in[i++];
    }
  }
  ret.setSize(j);
  return ret;
}