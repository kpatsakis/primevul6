size_t php_base64_encode(const unsigned char *str, int length,
                         unsigned char* outstr) {
  const unsigned char *current = str;
  unsigned char *p = outstr;

  while (length > 2) { /* keep going until we have less than 24 bits */
    *p++ = base64_table[current[0] >> 2];
    *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
    *p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
    *p++ = base64_table[current[2] & 0x3f];

    current += 3;
    length -= 3; /* we just handle 3 octets of data */
  }

  /* now deal with the tail end of things */
  if (length != 0) {
    *p++ = base64_table[current[0] >> 2];
    if (length > 1) {
      *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
      *p++ = base64_table[(current[1] & 0x0f) << 2];
      *p++ = base64_pad;
    } else {
      *p++ = base64_table[(current[0] & 0x03) << 4];
      *p++ = base64_pad;
      *p++ = base64_pad;
    }
  }
  return p - outstr;
}