do_vt52(uchar c)
{
  term_cursor *curs = &term.curs;
  term.state = NORMAL;
  term.autowrap = false;
  term.rev_wrap = false;
  term.esc_mod = 0;
  switch (c) {
    when '\e':
      term.state = ESCAPE;
    when '<':  /* Exit VT52 mode (Enter VT100 mode). */
      term.vt52_mode = 0;
    when '=':  /* Enter alternate keypad mode. */
      term.app_keypad = true;
    when '>':  /* Exit alternate keypad mode. */
      term.app_keypad = false;
    when 'A':  /* Cursor up. */
      move(curs->x, curs->y - 1, 0);
    when 'B':  /* Cursor down. */
      move(curs->x, curs->y + 1, 0);
    when 'C':  /* Cursor right. */
      move(curs->x + 1, curs->y, 0);
    when 'D':  /* Cursor left. */
      move(curs->x - 1, curs->y, 0);
    when 'F':  /* Enter graphics mode. */
      term.vt52_mode = 2;
    when 'G':  /* Exit graphics mode. */
      term.vt52_mode = 1;
    when 'H':  /* Move the cursor to the home position. */
      move(0, 0, 0);
    when 'I':  /* Reverse line feed. */
      if (curs->y == term.marg_top)
        term_do_scroll(term.marg_top, term.marg_bot, -1, true);
      else if (curs->y > 0)
        curs->y--;
      curs->wrapnext = false;
    when 'J':  /* Erase from the cursor to the end of the screen. */
      term_erase(false, false, false, true);
    when 'K':  /* Erase from the cursor to the end of the line. */
      term_erase(false, true, false, true);
    when 'Y':  /* Move the cursor to given row and column. */
      term.state = VT52_Y;
    when 'Z':  /* Identify. */
      child_write("\e/Z", 3);
    // Atari ST extensions
    when 'E':  /* Clear screen */
      move(0, 0, 0);
      term_erase(false, false, false, true);
    when 'b':  /* Foreground color */
      term.state = VT52_FG;
    when 'c':  /* Background color */
      term.state = VT52_BG;
    when 'd':  /* Clear to start of screen */
      term_erase(false, false, true, false);
    when 'e':  /* Enable cursor */
      term.cursor_on = true;
    when 'f':  /* Disable cursor */
      term.cursor_on = false;
    when 'j':  /* Save cursor */
      save_cursor();
    when 'k':  /* Restore cursor */
      restore_cursor();
    when 'l':  /* Clear line */
      term_erase(false, true, true, true);
      write_return();
    when 'o':  /* Clear to start of line */
      term_erase(false, true, true, false);
    when 'p':  /* Reverse video */
      term.curs.attr.attr |= ATTR_REVERSE;
    when 'q':  /* Normal video */
      term.curs.attr.attr &= ~ATTR_REVERSE;
    when 'v':  /* Wrap on */
      term.autowrap = true;
      term.curs.wrapnext = false;
    when 'w':  /* Wrap off */
      term.autowrap = false;
      term.curs.wrapnext = false;
  }
}