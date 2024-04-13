copy_rect(short y0, short x0, short y1, short x1, short y2, short x2)
{
  //printf("copy_rect %d,%d..%d,%d -> %d,%d\n", y0, x0, y1, x1, y2, x2);
  y0--; x0--; y1--; x1--; y2--; x2--;

  if (term.curs.origin) {
    y0 += term.marg_top;
    x0 += term.marg_left;
    y1 += term.marg_top;
    x1 += term.marg_left;
    y2 += term.marg_top;
    x2 += term.marg_left;
  }
  if (y0 < 0)
    y0 = 0;
  if (x0 < 0)
    x0 = 0;
  if (y1 >= term.rows)
    y1 = term.rows - 1;
  if (x1 >= term.cols)
    x1 = term.cols - 1;

  if (y2 < 0)
    y2 = 0;
  if (x2 < 0)
    x2 = 0;
  if (y2 + y1 - y0 >= term.rows)
    y1 = term.rows + y0 - y2 - 1;
  if (x2 + x1 - x0 >= term.cols)
    x1 = term.cols + x0 - x2 - 1;
  //printf("%d,%d..%d,%d -> %d,%d\n", y0, x0, y1, x1, y2, x2);

  bool down = y2 > y0;
  bool left = x2 > x0;
  for (int y = down ? y1 : y0; down ? y >= y0 : y <= y1; down ? y-- : y++) {
    termline * src = term.lines[y];
    termline * dst = term.lines[y + y2 - y0];
    term_check_boundary(x2, y + y2 - y0);
    term_check_boundary(x2 + x1 - x0 + 1, y + y2 - y0);
    for (int x = left ? x1 : x0; left ? x >= x0 : x <= x1; left ? x-- : x++) {
      copy_termchar(dst, x + x2 - x0, &src->chars[x]);
      //printf("copy %d:%d -> %d:%d\n", y, x, y + y2 - y0, x + x2 - x0);
      if ((x == x0 && src->chars[x].chr == UCSWIDE)
       || (x == x1 && charwidth(src->chars[x].chr) != 1)
         )
      {
        clear_cc(dst, x);
        dst->chars[x].chr = ' ';
      }
    }
  }
}