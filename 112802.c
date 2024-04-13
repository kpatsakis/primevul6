static char *php_ap_getword_conf(char **line) {
  char *str = *line, *strend, *res, quote;

  while (*str && isspace(*str)) {
    ++str;
  }

  if (!*str) {
    *line = str;
    return strdup("");
  }

  if ((quote = *str) == '"' || quote == '\'') {
    strend = str + 1;
look_for_quote:
    while (*strend && *strend != quote) {
      if (*strend == '\\' && strend[1] && strend[1] == quote) {
        strend += 2;
      } else {
        ++strend;
      }
    }
    if (*strend && *strend == quote) {
      char p = *(strend + 1);
      if (p != '\r' && p != '\n' && p != '\0') {
        strend++;
        goto look_for_quote;
      }
    }

    res = substring_conf(str + 1, strend - str - 1, quote);

    if (*strend == quote) {
      ++strend;
    }

  } else {

    strend = str;
    while (*strend && !isspace(*strend)) {
      ++strend;
    }
    res = substring_conf(str, strend - str, 0);
  }

  while (*strend && isspace(*strend)) {
    ++strend;
  }

  *line = strend;
  return res;
}