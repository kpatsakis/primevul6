insdel_column(int col, bool del, int n)
{
  //printf("insdel_column @%d %d marg %d..%d\n", col, n, term.marg_left, term.marg_right);
  int x0, x1, x2, e0, e1;
  if (del) {
    x0 = col + n;
    x1 = term.marg_right;
    x2 = col;
    e0 = term.marg_right - n + 1;
    e1 = term.marg_right;
    if (x0 > term.marg_right) {
      x0 = term.marg_right;
      e0 = col;
    }
  }
  else {
    if (col + n > term.marg_right + 1)
      n = term.marg_right + 1 - col;
    x0 = col;
    x1 = term.marg_right - n;
    x2 = col + n;
    e0 = col;
    e1 = col + n - 1;
  }
  x0++; x1++; x2++; e0++; e1++;
  int yt = term.marg_top + 1;
  int yb = term.marg_bot + 1;
  if (term.curs.origin) {
    // compensate for the originmode applied in the functions called below
    yt = 1;
    yb = term.marg_bot - term.marg_top + 1;
    x0 -= term.marg_left;
    x1 -= term.marg_left;
    x2 -= term.marg_left;
    e0 -= term.marg_left;
    e1 -= term.marg_left;
  }
  copy_rect(yt, x0, yb, x1, yt, x2);
  fill_rect(' ', term.curs.attr, false, yt, e0, yb, e1);
}