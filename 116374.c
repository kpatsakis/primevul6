sum_rect(short y0, short x0, short y1, short x1)
{
  //printf("sum_rect %d,%d..%d,%d\n", y0, x0, y1, x1);

  y0--; x0--; y1--; x1--;

  if (term.curs.origin) {
    y0 += term.marg_top;
    x0 += term.marg_left;
    y1 += term.marg_top;
    x1 += term.marg_left;
  }
  if (y0 < 0)
    y0 = 0;
  if (x0 < 0)
    x0 = 0;
  if (y1 >= term.rows)
    y1 = term.rows - 1;
  if (x1 >= term.cols)
    x1 = term.cols - 1;
  //printf("%d,%d..%d,%d\n", y0, x0, y1, x1);

  uint sum = 0;
  for (int y = y0; y <= y1; y++) {
    termline * line = term.lines[y];
    for (int x = x0; x <= x1; x++) {
      //printf("add %d:%d\n", y, x);
      if (line->chars[x].chr == UCSWIDE) {
      }
      else {
        sum += line->chars[x].chr;  // xterm default would mask & 0xFF
        cattrflags attr = line->chars[x].attr.attr;
        if (attr & ATTR_UNDER)
          sum += 0x10;
        else if (attr & ATTR_REVERSE)
          sum += 0x20;
        else if (attr & ATTR_BLINK)
          sum += 0x40;
        else if (attr & ATTR_BOLD)
          sum += 0x80;
        int xc = x;
        while (line->chars[xc].cc_next) {
          xc += line->chars[xc].cc_next;
          sum += line->chars[xc].chr & 0xFF;
        }
      }
    }
  }
  return sum;
}