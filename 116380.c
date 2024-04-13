do_vt52_colour(bool fg, uchar c)
{
  term.state = NORMAL;
  if (fg) {
    term.curs.attr.attr &= ~ATTR_FGMASK;
    term.curs.attr.attr |= ((c & 0xF) + ANSI0) << ATTR_FGSHIFT;
  }
  else {
    term.curs.attr.attr &= ~ATTR_BGMASK;
    term.curs.attr.attr |= ((c & 0xF) + ANSI0) << ATTR_BGSHIFT;
  }
}