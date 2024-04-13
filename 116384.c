save_cursor(void)
{
  term.saved_cursors[term.on_alt_screen] = term.curs;
}