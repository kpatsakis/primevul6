static int string_similar_char(const char *txt1, int len1,
                               const char *txt2, int len2) {
  int sum;
  int pos1 = 0, pos2 = 0, max;

  string_similar_str(txt1, len1, txt2, len2, &pos1, &pos2, &max);
  if ((sum = max)) {
    if (pos1 && pos2) {
      sum += string_similar_char(txt1, pos1, txt2, pos2);
    }
    if ((pos1 + max < len1) && (pos2 + max < len2)) {
      sum += string_similar_char(txt1 + pos1 + max, len1 - pos1 - max,
                                 txt2 + pos2 + max, len2 - pos2 - max);
    }
  }

  return sum;
}