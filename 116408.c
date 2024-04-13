write_linefeed(void)
{
  term_cursor *curs = &term.curs;
  if (curs->x < term.marg_left || curs->x > term.marg_right)
    return;

  clear_wrapcontd(term.lines[curs->y], curs->y);
  if (curs->y == term.marg_bot)
    term_do_scroll(term.marg_top, term.marg_bot, 1, true);
  else if (curs->y < term.rows - 1)
    curs->y++;
  curs->wrapnext = false;
}