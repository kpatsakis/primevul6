do_esc(uchar c)
{
  term_cursor *curs = &term.curs;
  term.state = NORMAL;

  // NRC designations
  // representation of NRC sequences at this point:
  //		term.esc_mod esc_mod0 esc_mod1 c
  // ESC)B	29 00 00 42
  // ESC)%5	FF 29 25 35
  // 94-character set designation as G0...G3: ()*+
  // 96-character set designation as G1...G3:  -./
  uchar designator = term.esc_mod == 0xFF ? esc_mod0 : term.esc_mod;
  uchar csmask = 0;
  int gi;
  if (designator) {
    void check_designa(char * designa, uchar cstype) {
      char * csdesigna = strchr(designa, designator);
      if (csdesigna) {
        csmask = cstype;
        gi = csdesigna - designa + cstype - 1;
      }
    }
    check_designa("()*+", 1);  // 94-character set designation?
    check_designa("-./", 2);  // 96-character set designation?
  }
  if (csmask) {
    ushort nrc_code = CPAIR(esc_mod1, c);
    term_cset cs = lookup_cset(nrc_code, csmask, term.decnrc_enabled);
    if (cs) {
      curs->csets[gi] = cs;
      term_update_cs();
      return;
    }
  }

  switch (CPAIR(term.esc_mod, c)) {
    when '[':  /* CSI: control sequence introducer */
      term.state = CSI_ARGS;
      term.csi_argc = 1;
      memset(term.csi_argv, 0, sizeof(term.csi_argv));
      memset(term.csi_argv_defined, 0, sizeof(term.csi_argv_defined));
      term.esc_mod = 0;
    when ']':  /* OSC: operating system command */
      term.state = OSC_START;
    when 'P':  /* DCS: device control string */
      term.state = DCS_START;
    when '^' or '_' or 'X': /* PM, APC, SOS strings to be ignored */
      term.state = IGNORE_STRING;
    when '7':  /* DECSC: save cursor */
      save_cursor();
    when '8':  /* DECRC: restore cursor */
      restore_cursor();
    when '=':  /* DECKPAM: Keypad application mode */
      term.app_keypad = true;
    when '>':  /* DECKPNM: Keypad numeric mode */
      term.app_keypad = false;
    when 'D':  /* IND: exactly equivalent to LF */
      write_linefeed();
    when 'E':  /* NEL: exactly equivalent to CR-LF */
      if (curs->x >= term.marg_left && curs->x <= term.marg_right) {
        write_return();
        write_linefeed();
      }
    when 'M':  /* RI: reverse index - backwards LF */
      if (curs->y == term.marg_top)
        term_do_scroll(term.marg_top, term.marg_bot, -1, true);
      else if (curs->y > 0)
        curs->y--;
      curs->wrapnext = false;
    when 'Z':  /* DECID: terminal type query */
      write_primary_da();
    when 'c':  /* RIS: restore power-on settings */
      winimgs_clear();
      term_reset(true);
      if (term.reset_132) {
        win_set_chars(term.rows, 80);
        term.reset_132 = 0;
      }
    when 'H':  /* HTS: set a tab */
      term.tabs[curs->x] = true;
    when 'l':  /* HP Memory Lock */
      if (curs->y < term.marg_bot)
        term.marg_top = curs->y;
    when 'm':  /* HP Memory Unlock */
      term.marg_top = 0;
    when CPAIR('#', '8'):    /* DECALN: fills screen with Es :-) */
      term.curs.origin = false;
      term.curs.wrapnext = false;
      term.curs.attr = CATTR_DEFAULT;
      term.marg_top = 0;
      term.marg_bot = term.rows - 1;
      term.marg_left = 0;
      term.marg_right = term.cols - 1;
      move(0, 0, 0);
      for (int i = 0; i < term.rows; i++) {
        termline *line = term.lines[i];
        for (int j = 0; j < term.cols; j++) {
          line->chars[j] =
            (termchar) {.cc_next = 0, .chr = 'E', .attr = CATTR_DEFAULT};
        }
        line->lattr = LATTR_NORM;
      }
      term.disptop = 0;
    when CPAIR('#', '3'):  /* DECDHL: 2*height, top */
      if (!term.lrmargmode) {
        term.lines[curs->y]->lattr &= LATTR_BIDIMASK;
        term.lines[curs->y]->lattr |= LATTR_TOP;
      }
    when CPAIR('#', '4'):  /* DECDHL: 2*height, bottom */
      if (!term.lrmargmode) {
        term.lines[curs->y]->lattr &= LATTR_BIDIMASK;
        term.lines[curs->y]->lattr |= LATTR_BOT;
      }
    when CPAIR('#', '5'):  /* DECSWL: normal */
      term.lines[curs->y]->lattr &= LATTR_BIDIMASK;
      term.lines[curs->y]->lattr |= LATTR_NORM;
    when CPAIR('#', '6'):  /* DECDWL: 2*width */
      if (!term.lrmargmode) {
        term.lines[curs->y]->lattr &= LATTR_BIDIMASK;
        term.lines[curs->y]->lattr |= LATTR_WIDE;
      }
    when CPAIR('%', '8') or CPAIR('%', 'G'):
      curs->utf = true;
      term_update_cs();
    when CPAIR('%', '@'):
      curs->utf = false;
      term_update_cs();
    when 'n':  /* LS2: Invoke G2 character set as GL */
      term.curs.gl = 2;
      term_update_cs();
    when 'o':  /* LS3: Invoke G3 character set as GL */
      term.curs.gl = 3;
      term_update_cs();
    when '~':  /* LS1R: Invoke G1 character set as GR */
      term.curs.gr = 1;
      term_update_cs();
    when '}':  /* LS2R: Invoke G2 character set as GR */
      term.curs.gr = 2;
      term_update_cs();
    when '|':  /* LS3R: Invoke G3 character set as GR */
      term.curs.gr = 3;
      term_update_cs();
    when 'N':  /* SS2: Single Shift G2 character set */
      term.curs.cset_single = curs->csets[2];
    when 'O':  /* SS3: Single Shift G3 character set */
      term.curs.cset_single = curs->csets[3];
    when '6':  /* Back Index (DECBI), VT420 */
      if (curs->x == term.marg_left)
        insdel_column(term.marg_left, false, 1);
      else
        move(curs->x - 1, curs->y, 1);
    when '9':  /* Forward Index (DECFI), VT420 */
      if (curs->x == term.marg_right)
        insdel_column(term.marg_left, true, 1);
      else
        move(curs->x + 1, curs->y, 1);
  }
}