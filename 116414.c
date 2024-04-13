write_tab(void)
{
  term_cursor *curs = &term.curs;

  int last = -1;
  do {
    if (curs->x == term.marg_right)
      break;
    last = curs->x;
    if (term.lines[curs->y]->chars[last].chr == ' '
        && (term.lines[curs->y]->chars[last].attr.attr & TATTR_CLEAR)
       )
      term.lines[curs->y]->chars[last].attr.attr |= ATTR_DIM;
    curs->x++;
  } while (curs->x < term.cols - 1 && !term.tabs[curs->x]);
  if (last >= 0
      && term.lines[curs->y]->chars[last].chr == ' '
      && (term.lines[curs->y]->chars[last].attr.attr & TATTR_CLEAR)
     )
    term.lines[curs->y]->chars[last].attr.attr |= ATTR_BOLD;

  if ((term.lines[curs->y]->lattr & LATTR_MODE) != LATTR_NORM) {
    if (curs->x >= term.cols / 2)
      curs->x = term.cols / 2 - 1;
  }
  else {
    if (curs->x >= term.cols)
      curs->x = term.cols - 1;
  }
}