inline StringRef thousands_sep(
    LConv *lc, LConvCheck<char *LConv::*, &LConv::thousands_sep> = 0) {
  return lc->thousands_sep;
}