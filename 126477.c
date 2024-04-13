static void remove_double_dots(char *s) {
  char *p = s;
  while (*s != '\0') {
    *p++ = *s++;
    if (s[-1] == '/' || s[-1] == '\\') {
      while (s[0] != '\0') {
        if (s[0] == '/' || s[0] == '\\') {
          s++;
        } else if (s[0] == '.' && s[1] == '.' &&
                   (s[2] == '/' || s[2] == '\\')) {
          s += 2;
        } else {
          break;
        }
      }
    }
  }
  *p = '\0';
}