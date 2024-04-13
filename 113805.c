onig_strncmp(const UChar* s1, const UChar* s2, int n)
{
  int x;

  while (n-- > 0) {
    x = *s2++ - *s1++;
    if (x) return x;
  }
  return 0;
}