String string_pad(const char *input, int len, int pad_length,
                  const char *pad_string, int pad_str_len,
                  int pad_type) {
  assertx(input);
  int num_pad_chars = pad_length - len;

  /* If resulting string turns out to be shorter than input string,
     we simply copy the input and return. */
  if (pad_length < 0 || num_pad_chars < 0) {
    return String(input, len, CopyString);
  }

  /* Setup the padding string values if specified. */
  if (pad_str_len == 0) {
    throw_invalid_argument("pad_string: (empty)");
    return String();
  }

  String ret(pad_length, ReserveString);
  char *result = ret.mutableData();

  /* We need to figure out the left/right padding lengths. */
  int left_pad, right_pad;
  switch (pad_type) {
  case STR_PAD_RIGHT:
    left_pad = 0;
    right_pad = num_pad_chars;
    break;
  case STR_PAD_LEFT:
    left_pad = num_pad_chars;
    right_pad = 0;
    break;
  case STR_PAD_BOTH:
    left_pad = num_pad_chars / 2;
    right_pad = num_pad_chars - left_pad;
    break;
  default:
    throw_invalid_argument("pad_type: %d", pad_type);
    return String();
  }

  /* First we pad on the left. */
  int result_len = 0;
  for (int i = 0; i < left_pad; i++) {
    result[result_len++] = pad_string[i % pad_str_len];
  }

  /* Then we copy the input string. */
  memcpy(result + result_len, input, len);
  result_len += len;

  /* Finally, we pad on the right. */
  for (int i = 0; i < right_pad; i++) {
    result[result_len++] = pad_string[i % pad_str_len];
  }
  ret.setSize(result_len);
  return ret;
}