write_ucschar(wchar hwc, wchar wc, int width)
{
  cattrflags attr = term.curs.attr.attr;
  ucschar c = hwc ? combine_surrogates(hwc, wc) : wc;
  uchar cf = scriptfont(c);
#ifdef debug_scriptfonts
  if (c && (cf || c > 0xFF))
    printf("write_ucschar %04X scriptfont %d\n", c, cf);
#endif
  if (cf && cf <= 10 && !(attr & FONTFAM_MASK))
    term.curs.attr.attr = attr | ((cattrflags)cf << ATTR_FONTFAM_SHIFT);

  if (hwc) {
    if (width == 1
        && (cfg.charwidth == 10 || cs_single_forced)
        && (is_wide(c) || (cs_ambig_wide && is_ambig(c)))
       )
    { // ensure indication of cjksingle width handling to trigger down-zooming
      width = 2;
    }
    write_char(hwc, width);
    write_char(wc, -1);  // -1 indicates low surrogate
  }
  else
    write_char(wc, width);

  term.curs.attr.attr = attr;
}