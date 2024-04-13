insert_char(int n)
{
  if (term.curs.x < term.marg_left || term.curs.x > term.marg_right)
    return;

  bool del = n < 0;
  int m;
  term_cursor *curs = &term.curs;
  termline *line = term.lines[curs->y];
  int cols = min(line->cols, line->size);
  cols = min(cols, term.marg_right + 1);

  n = (n < 0 ? -n : n);
  if (n > cols - curs->x)
    n = cols - curs->x;
  m = cols - curs->x - n;
  term_check_boundary(curs->x, curs->y);
  term_check_boundary(curs->x + m, curs->y);
  if (del) {
    for (int j = 0; j < m; j++)
      move_termchar(line, line->chars + curs->x + j,
                    line->chars + curs->x + j + n);
    while (n--)
      line->chars[curs->x + m++] = term.erase_char;
  }
  else {
    for (int j = m; j--;)
      move_termchar(line, line->chars + curs->x + j + n,
                    line->chars + curs->x + j);
    while (n--)
      line->chars[curs->x + n] = term.erase_char;
  }
}