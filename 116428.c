mapfont(struct rangefont * ranges, uint len, char * script, uchar f)
{
  for (uint i = 0; i < len; i++) {
    if (0 == strcmp(ranges[i].scriptname, script))
      ranges[i].font = f;
  }
  if (0 == strcmp(script, "CJK")) {
    mapfont(ranges, len, "Han", f);
    mapfont(ranges, len, "Hangul", f);
    mapfont(ranges, len, "Katakana", f);
    mapfont(ranges, len, "Hiragana", f);
    mapfont(ranges, len, "Bopomofo", f);
    mapfont(ranges, len, "Kanbun", f);
    mapfont(ranges, len, "Fullwidth", f);
    mapfont(ranges, len, "Halfwidth", f);
  }
}