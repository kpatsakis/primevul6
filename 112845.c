String string_escape_shell_arg(const char *str) {
  int x, y, l;
  char *cmd;

  y = 0;
  l = strlen(str);

  String ret(safe_address(l, 4, 3), ReserveString); /* worst case */
  cmd = ret.mutableData();

#ifdef _MSC_VER
  cmd[y++] = '"';
#else
  cmd[y++] = '\'';
#endif

  for (x = 0; x < l; x++) {
    switch (str[x]) {
#ifdef _MSC_VER
    case '"':
    case '%':
    case '!':
      cmd[y++] = ' ';
      break;
#else
    case '\'':
      cmd[y++] = '\'';
      cmd[y++] = '\\';
      cmd[y++] = '\'';
#endif
      /* fall-through */
    default:
      cmd[y++] = str[x];
    }
  }
#ifdef _MSC_VER
  if (y > 0 && '\\' == cmd[y - 1]) {
    int k = 0, n = y - 1;
    for (; n >= 0 && '\\' == cmd[n]; n--, k++);
    if (k % 2) {
      cmd[y++] = '\\';
    }
  }

  cmd[y++] = '"';
#else
  cmd[y++] = '\'';
#endif
  ret.setSize(y);
  return ret;
}