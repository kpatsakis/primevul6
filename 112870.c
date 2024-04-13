String string_escape_shell_cmd(const char *str) {
  register int x, y, l;
  char *cmd;
  char *p = nullptr;

  l = strlen(str);
  String ret(safe_address(l, 2, 1), ReserveString);
  cmd = ret.mutableData();

  for (x = 0, y = 0; x < l; x++) {
    switch (str[x]) {
#ifndef _MSC_VER
    case '"':
    case '\'':
      if (!p && (p = (char *)memchr(str + x + 1, str[x], l - x - 1))) {
        /* noop */
      } else if (p && *p == str[x]) {
        p = nullptr;
      } else {
        cmd[y++] = '\\';
      }
      cmd[y++] = str[x];
      break;
#else
    /* % is Windows specific for environmental variables, ^%PATH% will
    output PATH while ^%PATH^% will not. escapeshellcmd->val will
    escape all % and !.
    */
    case '%':
    case '!':
    case '"':
    case '\'':
#endif
    case '#': /* This is character-set independent */
    case '&':
    case ';':
    case '`':
    case '|':
    case '*':
    case '?':
    case '~':
    case '<':
    case '>':
    case '^':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '$':
    case '\\':
    case '\x0A': /* excluding these two */
    case '\xFF':
#ifdef _MSC_VER
      cmd[y++] = '^';
#else
      cmd[y++] = '\\';
#endif
      /* fall-through */
    default:
      cmd[y++] = str[x];
    }
  }
  ret.setSize(y);
  return ret;
}