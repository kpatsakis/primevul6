attr_rect(cattrflags add, cattrflags sub, cattrflags xor, short y0, short x0, short y1, short x1)
{
  //printf("attr_rect %d,%d..%d,%d +%llX -%llX ^%llX\n", y0, x0, y1, x1, add, sub, xor);
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

  for (int y = y0; y <= y1; y++) {
    termline * l = term.lines[y];
    int xl = x0;
    int xr = x1;
    if (!term.attr_rect) {
      if (y != y0)
        xl = term.marg_left;
      if (y != y1)
        xr = term.marg_right;
    }
    for (int x = xl; x <= xr; x++) {
      //printf("attr %d:%d\n", y, x);
      cattrflags ca = l->chars[x].attr.attr;
      ca ^= xor;
      ca &= ~sub;
      ca |= add;
      if (ca != l->chars[x].attr.attr) {
        if (x == xl)
          term_check_boundary(x, y);
        if (x == xr)
          term_check_boundary(x + 1, y);
      }
      l->chars[x].attr.attr = ca;
    }
  }
}