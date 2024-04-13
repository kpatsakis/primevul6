write_char(wchar c, int width)
{
  //if (kb_trace) printf("[%ld] write_char 'q'\n", mtime());

  if (tek_mode) {
    tek_write(c, width);
    return;
  }

  if (!c)
    return;

  term_cursor * curs = &term.curs;
  termline * line = term.lines[curs->y];

  // support non-BMP for the REP function;
  // this is a hack, it would be cleaner to fold the term_write block
  //   switch (term.state) when NORMAL:
  // and repeat that
  if (width == -1) {  // low surrogate
    last_high = last_char;
  }
  else {
    last_high = 0;
    last_width = width;
  }
  last_char = c;
  last_attr = curs->attr;

  void wrapparabidi(ushort parabidi, termline * line, int y)
  {
    line->lattr = (line->lattr & ~LATTR_BIDIMASK) | parabidi | LATTR_WRAPCONTD;

#ifdef determine_parabidi_during_output
    if (parabidi & (LATTR_BIDISEL | LATTR_AUTOSEL))
      return;

    // if direction autodetection pending:
    // from current line, extend backward and forward to adjust 
    // "paragraph" bidi attributes (esp. direction) to wrapped lines
    termline * paraline = line;
    int paray = y;
    while ((paraline->lattr & LATTR_WRAPCONTD) && paray > -sblines()) {
      paraline = fetch_line(--paray);
      paraline->lattr = (paraline->lattr & ~LATTR_BIDIMASK) | parabidi;
      release_line(paraline);
    }
    paraline = line;
    paray = y;
    while ((paraline->lattr & LATTR_WRAPPED) && paray < term.rows) {
      paraline = fetch_line(++paray);
      paraline->lattr = (paraline->lattr & ~LATTR_BIDIMASK) | parabidi;
      release_line(paraline);
    }
#else
    (void)y;
#endif
  }

  void put_char(wchar c)
  {
    if (term.ring_enabled && curs->x == term.marg_right + 1 - 8) {
      win_margin_bell(&cfg);
      term.ring_enabled = false;
    }

    clear_cc(line, curs->x);
    line->chars[curs->x].chr = c;
    line->chars[curs->x].attr = curs->attr;
#ifdef insufficient_approach
#warning this does not help when scrolling via rectangular copy
    if (term.lrmargmode)
      line->lattr &= ~LATTR_MODE;
#endif
    if (!(line->lattr & LATTR_WRAPCONTD))
      line->lattr = (line->lattr & ~LATTR_BIDIMASK) | curs->bidimode;
    //TODO: if changed, propagate mode onto paragraph
    if (cfg.ligatures_support)
      term_invalidate(0, curs->y, curs->x, curs->y);
  }

  if (curs->wrapnext && term.autowrap && width > 0) {
    line->lattr |= LATTR_WRAPPED;
    line->wrappos = curs->x;
    ushort parabidi = getparabidi(line);
    if (curs->y == term.marg_bot)
      term_do_scroll(term.marg_top, term.marg_bot, 1, true);
    else if (curs->y < term.rows - 1)
      curs->y++;
    curs->x = term.marg_left;
    curs->wrapnext = false;
    line = term.lines[curs->y];
    wrapparabidi(parabidi, line, curs->y);
  }

  bool overstrike = false;
  if (curs->attr.attr & ATTR_OVERSTRIKE) {
    width = 0;
    overstrike = true;
    curs->wrapnext = false;
  }

  bool single_width = false;

  // adjust to explicit width attribute; not for combinings and low surrogates
  if (curs->width && width > 0) {
    //if ((c & 0xFFF) == 0x153) printf("%llX %d\n", curs->attr.attr, width);
    if (curs->width == 1) {
      if (!(width < 2 || (cs_ambig_wide && is_ambig(c))))
        curs->attr.attr |= TATTR_CLEAR | TATTR_NARROW;
      width = 1;
    }
    else if (curs->width == 11) {
      if (width > 1) {
        if (!(cs_ambig_wide && is_ambig(c))) {
          single_width = true;
          curs->attr.attr |= TATTR_SINGLE;
        }
        width = 1;
      }
    }
    else if (curs->width == 2) {
      if (width < 2) {
        curs->attr.attr |= TATTR_EXPAND;
        width = 2;
      }
    }
#ifdef support_triple_width
    else if (curs->width == 3) {
      if (width < 2 || (cs_ambig_wide && is_ambig(c)))
        curs->attr.attr |= TATTR_EXPAND;
#define TATTR_TRIPLE 0x0080000000000000u
      curs->attr.attr |= TATTR_TRIPLE;
      width = 3;
    }
#endif
  }

#ifdef enforce_ambiguous_narrow_here
  // enforce ambiguous-narrow as configured or for WSL;
  // this could be done here but is now sufficiently achieved in charset.c
  if (cs_ambig_narrow && width > 1 && is_ambig(c))
    width = 1;
#endif

  if (cfg.charwidth >= 10 || cs_single_forced) {
    if (width > 1) {
      single_width = true;
      width = 1;
    }
    else if (is_wide(c) || (cs_ambig_wide && is_ambig(c))) {
      single_width = true;
    }
  }

  if (term.insert && width > 0)
    insert_char(width);

  switch (width) {
    when 1:  // Normal character.
      term_check_boundary(curs->x, curs->y);
      term_check_boundary(curs->x + 1, curs->y);
      put_char(c);
      if (single_width)
        line->chars[curs->x].attr.attr |= TATTR_SINGLE;
    when 2 or 3:  // Double-width char (Triple-width was an experimental option).
     /*
      * If we're about to display a double-width character 
      * starting in the rightmost column, 
      * then we do something special instead.
      * We must print a space in the last column of the screen, then wrap;
      * and we also set LATTR_WRAPPED2 which instructs subsequent 
      * cut-and-pasting not only to splice this line to the one after it, 
      * but to ignore the space in the last character position as well.
      * (Because what was actually output to the terminal was presumably 
      * just a sequence of CJK characters, and we don't want a space to be
      * pasted in the middle of those just because they had the misfortune 
      * to start in the wrong parity column. xterm concurs.)
      */
      term_check_boundary(curs->x, curs->y);
      term_check_boundary(curs->x + width, curs->y);
      if (curs->x == term.marg_right || curs->x == term.cols - 1) {
        line->chars[curs->x] = term.erase_char;
        line->lattr |= LATTR_WRAPPED | LATTR_WRAPPED2;
        line->wrappos = curs->x;
        ushort parabidi = getparabidi(line);
        if (curs->y == term.marg_bot)
          term_do_scroll(term.marg_top, term.marg_bot, 1, true);
        else if (curs->y < term.rows - 1)
          curs->y++;
        curs->x = term.marg_left;
        line = term.lines[curs->y];
        wrapparabidi(parabidi, line, curs->y);
       /* Now we must term_check_boundary again, of course. */
        term_check_boundary(curs->x, curs->y);
        term_check_boundary(curs->x + width, curs->y);
      }
      put_char(c);
      curs->x++;
      put_char(UCSWIDE);
#ifdef support_triple_width
      if (width > 2) {
        for (int i = 2; i < width; i++) {
          curs->x++;
          put_char(UCSWIDE);
        }
      }
#endif
    when 0 or -1:  // Combining character or Low surrogate.
#ifdef debug_surrogates
      printf("write_char %04X %2d %08llX\n", c, width, curs->attr.attr);
#endif
      if (curs->x > 0 || overstrike) {
       /* If we're in wrapnext state, the character
        * to combine with is _here_, not to our left. */
        int x = curs->x - !curs->wrapnext;
       /* Same if we overstrike an actually not combining character. */
        if (overstrike)
          x = curs->x;
       /*
        * If the previous character is UCSWIDE, back up another one.
        */
        if (line->chars[x].chr == UCSWIDE) {
          assert(x > 0);
          x--;
        }
       /* Try to precompose with the cell's base codepoint */
        wchar pc;
        if (termattrs_equal_fg(&line->chars[x].attr, &curs->attr))
          pc = win_combine_chars(line->chars[x].chr, c, curs->attr.attr);
        else
          pc = 0;
        if (pc)
          line->chars[x].chr = pc;
        else
          add_cc(line, x, c, curs->attr);
      }
      else {
        // add initial combining characters, 
        // particularly to include initial bidi directional markers
        add_cc(line, -1, c, curs->attr);
      }
      if (!overstrike)
        return;
      // otherwise width 0 was faked for this switch, 
      // and we still need to advance the cursor below
    otherwise:  // Anything else. Probably shouldn't get here.
      return;
  }

  curs->x++;
  if (curs->x == term.marg_right + 1 || curs->x == term.cols) {
    curs->x--;
    if (term.autowrap || cfg.old_wrapmodes)
      curs->wrapnext = true;
  }
}