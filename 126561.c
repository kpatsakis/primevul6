bool mg_globmatch(const char *s1, size_t n1, const char *s2, size_t n2) {
  size_t i = 0, j = 0, ni = 0, nj = 0;
  while (i < n1 || j < n2) {
    if (i < n1 && j < n2 && (s1[i] == '?' || s2[j] == s1[i])) {
      i++, j++;
    } else if (i < n1 && (s1[i] == '*' || s1[i] == '#')) {
      ni = i, nj = j + 1, i++;
    } else if (nj > 0 && nj <= n2 && (s1[ni] == '#' || s2[j] != '/')) {
      i = ni, j = nj;
    } else {
      // printf(">>: [%s] [%s] %d %d %d %d\n", s1, s2, i, j, ni, nj);
      return false;
    }
  }
  return true;
}