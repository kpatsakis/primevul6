write_backspace(void)
{
  term_cursor *curs = &term.curs;
  if (curs->x == term.marg_left && curs->y == term.marg_top
      && term.rev_wrap && !cfg.old_wrapmodes
     )
  {
    curs->y = term.marg_bot;
    curs->x = term.marg_right;
  }
  else if (curs->x == 0 && (curs->y == term.marg_top || !term.autowrap
                       || (!cfg.old_wrapmodes && !term.rev_wrap)))
    /* skip */;
  else if (curs->x == term.marg_left && curs->y > term.marg_top) {
    curs->y--;
    curs->x = term.marg_right;
  }
  else if (curs->wrapnext) {
    curs->wrapnext = false;
    if (!term.rev_wrap && !cfg.old_wrapmodes)
      curs->x--;
  }
  else if (curs->x > 0 && curs->x != term.marg_left)
    curs->x--;
}