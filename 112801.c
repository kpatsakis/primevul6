static char *substring_conf(char *start, int len, char quote) {
  char *result = (char *)malloc(len + 2);
  char *resp = result;
  int i;

  for (i = 0; i < len; ++i) {
    if (start[i] == '\\' &&
        (start[i + 1] == '\\' || (quote && start[i + 1] == quote))) {
      *resp++ = start[++i];
    } else {
      *resp++ = start[i];
    }
  }

  *resp++ = '\0';
  return result;
}