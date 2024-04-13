do_ctrl(char c)
{
  if (tek_mode) {
    tek_ctrl(c);
    return true;
  }

  switch (c) {
    when '\e':   /* ESC: Escape */
      term.state = ESCAPE;
      term.esc_mod = 0;
    when '\a':   /* BEL: Bell */
      write_bell();
    when '\b':     /* BS: Back space */
      write_backspace();
    when '\t':     /* HT: Character tabulation */
      write_tab();
    when '\v':   /* VT: Line tabulation */
      write_linefeed();
      if (term.newline_mode)
        write_return();
    when '\f':   /* FF: Form feed */
      write_linefeed();
      if (term.newline_mode)
        write_return();
    when '\r':   /* CR: Carriage return */
      write_return();
    when '\n':   /* LF: Line feed */
      write_linefeed();
      if (term.newline_mode)
        write_return();
    when CTRL('E'):   /* ENQ: terminal type query */
      if (!term.vt52_mode) {
        char * ab = cs__wcstombs(cfg.answerback);
        child_write(ab, strlen(ab));
        free(ab);
      }
    when CTRL('N'):   /* LS1: Locking-shift one */
      if (!term.vt52_mode) {
        term.curs.gl = 1;
        term_update_cs();
      }
    when CTRL('O'):   /* LS0: Locking-shift zero */
      if (!term.vt52_mode) {
        term.curs.gl = 0;
        term_update_cs();
      }
    otherwise:
      return false;
  }
  return true;
}