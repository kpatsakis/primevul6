move(int x, int y, int marg_clip)
{
  term_cursor *curs = &term.curs;

  if (marg_clip) {
    if ((curs->y >= term.marg_top || marg_clip == 2) && y < term.marg_top)
      y = term.marg_top;
    if ((curs->y <= term.marg_bot || marg_clip == 2) && y > term.marg_bot)
      y = term.marg_bot;
    if ((curs->x >= term.marg_left || marg_clip == 2) && x < term.marg_left)
      x = term.marg_left;
    if ((curs->x <= term.marg_right || marg_clip == 2) && x > term.marg_right)
      x = term.marg_right;
  }

  if (x < 0)
    x = 0;
  if (x >= term.cols)
    x = term.cols - 1;
  if (y < 0)
    y = 0;
  if (y >= term.rows)
    y = term.rows - 1;

  curs->x = x;
  curs->y = y;
  curs->wrapnext = false;
}