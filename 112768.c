int string_similar_text(const char *t1, int len1,
                        const char *t2, int len2, float *percent) {
  if (len1 == 0 && len2 == 0) {
    if (percent) *percent = 0.0;
    return 0;
  }

  int sim = string_similar_char(t1, len1, t2, len2);
  if (percent) *percent = sim * 200.0 / (len1 + len2);
  return sim;
}