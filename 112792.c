static char *php_ap_getword(char **line, char stop) {
  char *pos = *line, quote;
  char *res;

  while (*pos && *pos != stop) {

    if ((quote = *pos) == '"' || quote == '\'') {
      ++pos;
      while (*pos && *pos != quote) {
        if (*pos == '\\' && pos[1] && pos[1] == quote) {
          pos += 2;
        } else {
          ++pos;
        }
      }
      if (*pos) {
        ++pos;
      }
    } else ++pos;

  }
  if (*pos == '\0') {
    res = strdup(*line);
    *line += strlen(*line);
    return res;
  }

  res = (char*)malloc(pos - *line + 1);
  memcpy(res, *line, pos - *line);
  res[pos - *line] = 0;
  while (*pos == stop) {
    ++pos;
  }

  *line = pos;
  return res;
}