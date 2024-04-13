contains(string s, int i)
{
  while (*s) {
    while (*s == ',' || *s == ' ')
      s++;
    int si = -1;
    int len;
    if (sscanf(s, "%d%n", &si, &len) <= 0)
      return false;
    s += len;
    if (si == i && (!*s || *s == ',' || *s == ' '))
      return true;
  }
  return false;
}