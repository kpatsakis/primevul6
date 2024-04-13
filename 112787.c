static void string_similar_str(const char *txt1, int len1,
                               const char *txt2, int len2,
                               int *pos1, int *pos2, int *max) {
  const char *p, *q;
  const char *end1 = txt1 + len1;
  const char *end2 = txt2 + len2;
  int l;

  *max = 0;
  for (p = txt1; p < end1; p++) {
    for (q = txt2; q < end2; q++) {
      for (l = 0; (p + l < end1) && (q + l < end2) && (p[l] == q[l]); l++);
      if (l > *max) {
        *max = l;
        *pos1 = p - txt1;
        *pos2 = q - txt2;
      }
    }
  }
}