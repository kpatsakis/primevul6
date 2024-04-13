tek_esc(char c)
{
  if (prev_state)
    term.state = prev_state;
  else
    term.state = NORMAL;

  switch (c) {
    when '\e':   /* stay in ESC state */
      term.state = TEK_ESCAPE;
    when '\n':   /* LF: stay in ESC state */
      term.state = TEK_ESCAPE;
    when 0 or '\r':   /* stay in ESC state */
      term.state = TEK_ESCAPE;
    when '\a':   /* BEL: Bell */
      write_bell();
    when '\b' or '\t' or '\v':     /* BS or HT or VT */
      tek_write(c, -2);
    when CTRL('L'):   /* FF: Alpha mode, clear screen */
      tek_mode = TEKMODE_ALPHA;
      term.state = NORMAL;
      tek_bypass = false;
      tek_clear();
    when CTRL('E'):   /* ENQ: terminal type query */
      tek_bypass = true;
      tek_enq();
    when CTRL('N'):   /* LS1: Locking-shift one */
      tek_alt(true);
    when CTRL('O'):   /* LS0: Locking-shift zero */
      tek_alt(false);
    when CTRL('W'):   /* ETB: Make Copy */
      term_save_image();
      tek_bypass = false;
    when CTRL('X'):   /* CAN: Set Bypass */
      tek_bypass = true;
    when CTRL('Z'):   /* SUB: Gin mode */
      tek_mode = TEKMODE_GIN;
      tek_gin();
      term.state = NORMAL;
      tek_bypass = true;
    when 0x1C:   /* FS: Special Plot mode */
      tek_mode = TEKMODE_SPECIAL_PLOT;
      term.state = TEK_ADDRESS0;
    when 0x1D:   /* GS: Graph mode */
      tek_mode = TEKMODE_GRAPH0;
      term.state = TEK_ADDRESS0;
    when 0x1E:   /* RS: Incremental Plot mode */
      tek_mode = TEKMODE_INCREMENTAL_PLOT;
      term.state = TEK_INCREMENTAL;
    when 0x1F:   /* US: Normal mode */
      tek_mode = TEKMODE_ALPHA;
      term.state = NORMAL;
    when '`' ... 'g':  /* Normal mode */
      tek_beam(false, false, c & 7);
    when 'h' ... 'o':  /* Defocused mode */
      tek_beam(true, false, c & 7);
    when 'p' ... 'w':  /* Write-Thru mode */
      tek_beam(false, true, c & 7);
    when '8' ... ';':
      tek_font(c - '8');
    when '?':
      if (term.state == TEK_ADDRESS0 || term.state == TEK_ADDRESS)
        term_do_write("", 1);
    when CTRL('C'):
      tek_mode = TEKMODE_OFF;
      term.state = NORMAL;
      win_invalidate_all(false);
    when ']':  /* OSC: operating system command */
      term.state = OSC_START;
  }
}