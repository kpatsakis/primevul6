static inline size_t printCharTo(char c, Print &p) {
  char specialChar = getSpecialChar(c);

  return specialChar ? p.write('\\') + p.write(specialChar) : p.write(c);
}