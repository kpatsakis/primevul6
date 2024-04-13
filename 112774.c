String string_escape_shell_arg(const char *str) {
  int x, y, l;
  char *cmd;

  y = 0;
  l = strlen(str);

  String ret(safe_address(l, 4, 3), ReserveString); /* worst case */
  cmd = ret.bufferSlice().ptr;

  cmd[y++] = '\'';

  for (x = 0; x < l; x++) {
    switch (str[x]) {
    case '\'':
      cmd[y++] = '\'';
      cmd[y++] = '\\';
      cmd[y++] = '\'';
      /* fall-through */
    default:
      cmd[y++] = str[x];
    }
  }
  cmd[y++] = '\'';
  ret.setSize(y);
  return ret;
}