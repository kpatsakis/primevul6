tek_ctrl(char c)
{
  if (term.state == TEK_ADDRESS0 || term.state == TEK_ADDRESS)
    prev_state = term.state;

  switch (c) {
    when '\e':   /* ESC: Escape */
      prev_state = term.state;
      term.state = TEK_ESCAPE;
    when '\a':   /* BEL: Bell */
      write_bell();
      tek_bypass = false;
    when '\b' or '\t' or '\v':     /* BS or HT or VT */
      if (tek_mode == TEKMODE_ALPHA)
        tek_write(c, -2);
    when '\n':   /* LF: Line feed */
      tek_bypass = false;
      tek_write(c, -2);
    when '\r':   /* CR: Carriage return */
      tek_mode = TEKMODE_ALPHA;
      term.state = NORMAL;
      tek_bypass = false;
      tek_write(c, -2);
    when 0x1C:   /* FS: Point Plot mode */
      tek_mode = TEKMODE_POINT_PLOT;
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
      tek_bypass = false;
  }
}