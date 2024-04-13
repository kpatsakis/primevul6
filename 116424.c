restore_cursor(void)
{
  term_cursor *curs = &term.curs;
  *curs = term.saved_cursors[term.on_alt_screen];
  term.erase_char.attr = curs->attr;
  term.erase_char.attr.attr &= (ATTR_FGMASK | ATTR_BGMASK);
  term.erase_char.attr.attr |= TATTR_CLEAR;

 /* Make sure the window hasn't shrunk since the save */
  if (curs->x >= term.cols)
    curs->x = term.cols - 1;
  if (curs->y >= term.rows)
    curs->y = term.rows - 1;

 /* In origin mode, make sure the cursor position is within margins */
  if (curs->origin) {
    if (curs->x < term.marg_left)
      curs->x = term.marg_left;
    else if (curs->x > term.marg_right)
      curs->x = term.marg_right;
    if (curs->y < term.marg_top)
      curs->y = term.marg_top;
    else if (curs->y > term.marg_bot)
      curs->y = term.marg_bot;
  }

 /*
  * wrapnext might reset to False 
  * if the x position is no longer at the rightmost edge.
  */
  if (curs->wrapnext && curs->x < term.cols - 1 && curs->x != term.marg_right)
    curs->wrapnext = false;

  term_update_cs();
}