fill_rect(xchar chr, cattr attr, bool sel, short y0, short x0, short y1, short x1)
{
  //printf("fill_rect %d,%d..%d,%d\n", y0, x0, y1, x1);
  int width = charwidth(chr);
  if (chr == UCSWIDE || width < 1)
    return;
  wchar low = 0;
  if (chr > 0xFFFF) {
    low = low_surrogate(chr);
    chr = high_surrogate(chr);
  }

  y0--; x0--; y1--; x1--;

  if (term.curs.origin) {
    y0 += term.marg_top;
    x0 += term.marg_left;
    y1 += term.marg_top;
    x1 += term.marg_left;
  }
  if (y0 < 0)
    y0 = 0;
  if (x0 < 0)
    x0 = 0;
  if (y1 >= term.rows)
    y1 = term.rows - 1;
  if (x1 >= term.cols)
    x1 = term.cols - 1;
  //printf("%d,%d..%d,%d\n", y0, x0, y1, x1);

  //printf("gl %d gr %d csets %d %d %d %d /%d sup %d acs %d\n", term.curs.gl, term.curs.gr, term.curs.csets[0], term.curs.csets[1], term.curs.csets[2], term.curs.csets[3], term.curs.cset_single, term.curs.decsupp, term.curs.oem_acs);
  if ((chr > ' ' && chr < 0x80 
       && (term.curs.csets[term.curs.gl] != CSET_ASCII
           ||
           term.curs.cset_single != CSET_ASCII
          )
      )
      ||
      (chr >= 0x80 && chr < 0x100 
       && ((term.curs.gr && term.curs.csets[term.curs.gr] != CSET_ASCII)
           || term.curs.oem_acs
          )
      )
      || (chr >= 0x2580 && chr <= 0x259F)
     )
  {
    term_cursor csav = term.curs;
    term.curs.attr = attr;
#ifdef debug_FRA_special
    // make this code branch visible
    term.curs.attr.attr &= ~ATTR_FGMASK;
    term.curs.attr.attr |= RED_I << ATTR_FGSHIFT;
#endif
    term.curs.width = 1;
    if (!(width < 2 || (cs_ambig_wide && is_ambig(chr))))
      term.curs.attr.attr |= TATTR_CLEAR | TATTR_NARROW;
    term.state = NORMAL;

    char * cbuf = 0;
    if (chr > 0xFF) {
      wchar * wc = (wchar[]){chr, low, 0};
      cbuf = cs__wcstombs(wc);
    }
    for (int y = y0; y <= y1; y++) {
      term.curs.y = y;
      for (int x = x0; x <= x1; x++) {
        term.curs.x = x;
        term.curs.cset_single = csav.cset_single;
        if (chr > 0xFF) {
          //write_char(chr, 1); // would skip NRCS handling in term_do_write
          term_do_write(cbuf, strlen(cbuf));
        }
        else {
          char c = chr;
          term_do_write(&c, 1);
        }
      }
    }
    if (cbuf)
      free(cbuf);

    term.curs = csav;
    term.curs.cset_single = CSET_ASCII;
    return;
  }

  if (width > 1)
    attr.attr |= TATTR_CLEAR | TATTR_NARROW;

  for (int y = y0; y <= y1; y++) {
    termline * l = term.lines[y];
    bool prevprot = true;  // not false!
    for (int x = x0; x <= x1; x++) {
      //printf("fill %d:%d\n", y, x);
      bool prot = sel && l->chars[x].attr.attr & ATTR_PROTECTED;
      if (prot != prevprot) {
        // |P not here, no check
        // |N check
        // NP check only current position
        // PN check
        if (!prot) {  // includes the case x == x0
          // clear previous half of wide char, even if protected
          term_check_boundary(x0, y);
        }
        else if (l->chars[x].chr == UCSWIDE) {
          // clear right half of wide char, even if protected;
          // calling term_check_boundary would overwrite previous fill char
          clear_cc(l, x);
          l->chars[x].chr = ' ';
        }
      }
      // clear wide char on right area border unless protected
      if (!prot && x == x1)
        term_check_boundary(x1 + 1, y);
      prevprot = prot;

      if (!sel || !prot) {
        clear_cc(l, x);
        l->chars[x].chr = chr;
        l->chars[x].attr = attr;
        if (low)
          add_cc(l, x, low, attr);
      }
    }
  }
}