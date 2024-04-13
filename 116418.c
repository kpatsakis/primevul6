do_csi(uchar c)
{
  term_cursor *curs = &term.curs;
  int arg0 = term.csi_argv[0], arg1 = term.csi_argv[1];
  if (arg0 < 0)
    arg0 = 0;
  if (arg1 < 0)
    arg1 = 0;
  int arg0_def1 = arg0 ?: 1;  // first arg with default 1

  // DECRQM quirk
  if (term.esc_mod == 0xFF && esc_mod0 == '?' && esc_mod1 == '$' && c == 'p')
    term.esc_mod = '$';

  switch (CPAIR(term.esc_mod, c)) {
    when CPAIR('!', 'p'):     /* DECSTR: soft terminal reset */
      term_reset(false);
    when 'b': {      /* REP: repeat preceding character */
      cattr cur_attr = term.curs.attr;
      term.curs.attr = last_attr;
      wchar h = last_high, c = last_char;
      for (int i = 0; i < arg0_def1; i++)
        write_ucschar(h, c, last_width);
      term.curs.attr = cur_attr;
    }
    when 'A':        /* CUU: move up N lines */
      move(curs->x, curs->y - arg0_def1, 1);
    when 'e':        /* VPR: move down N lines */
      move(curs->x, curs->y + arg0_def1, 1);
    when 'B':        /* CUD: Cursor down */
      move(curs->x, curs->y + arg0_def1, 1);
    when 'c':        /* Primary DA: report device/terminal type */
      if (!arg0)
        write_primary_da();
    when CPAIR('>', 'c'):     /* Secondary DA: report device version */
      if (!arg0) {
        if (cfg.charwidth % 10)
          child_printf("\e[>77;%u;%uc", DECIMAL_VERSION, UNICODE_VERSION);
        else
          child_printf("\e[>77;%u;0c", DECIMAL_VERSION);
      }
    when CPAIR('>', 'q'):     /* Report terminal name and version */
      if (!arg0)
        child_printf("\eP>|%s %s\e\\", APPNAME, VERSION);
    when 'a':        /* HPR: move right N cols */
      move(curs->x + arg0_def1, curs->y, 1);
    when 'C':        /* CUF: Cursor right */
      move(curs->x + arg0_def1, curs->y, 1);
    when 'D':        /* CUB: move left N cols */
      if (arg0_def1 > curs->x) {
        arg0_def1 -= curs->x + 1;
        move(0, curs->y, 1);
        write_backspace();
        move(curs->x - arg0_def1, curs->y, 1);
      }
      else
        move(curs->x - arg0_def1, curs->y, 1);
      enable_progress();
    when 'E':        /* CNL: move down N lines and CR */
      move(0, curs->y + arg0_def1, 1);
    when 'F':        /* CPL: move up N lines and CR */
      move(0, curs->y - arg0_def1, 1);
    when 'G' or '`': { /* CHA or HPA: set horizontal position */
      short x = (curs->origin ? term.marg_left : 0) + arg0_def1 - 1;
      if (x < curs->x)
        enable_progress();
      move(x, curs->y, curs->origin ? 2 : 0);
    }
    when 'd':        /* VPA: set vertical position */
      move(curs->x,
           (curs->origin ? term.marg_top : 0) + arg0_def1 - 1,
           curs->origin ? 2 : 0);
    when 'H' or 'f':  /* CUP or HVP: set horiz. and vert. positions at once */
      move((curs->origin ? term.marg_left : 0) + (arg1 ?: 1) - 1,
           (curs->origin ? term.marg_top : 0) + arg0_def1 - 1,
           curs->origin ? 2 : 0);
    when 'I':  /* CHT: move right N TABs */
      for (int i = 0; i < arg0_def1; i++)
        write_tab();
    when 'J' or CPAIR('?', 'J'):  /* ED/DECSED: (selective) erase in display */
      if (arg0 == 3) { /* Erase Saved Lines (xterm) */
        // don't care if (term.esc_mod) // ignore selective
        term_clear_scrollback();
        term.disptop = 0;
      }
      else if (arg0 <= 2) {
        bool above = arg0 == 1 || arg0 == 2;
        bool below = arg0 == 0 || arg0 == 2;
        term_erase(term.esc_mod, false, above, below);
      }
    when 'K' or CPAIR('?', 'K'):  /* EL/DECSEL: (selective) erase in line */
      if (arg0 <= 2) {
        bool right = arg0 == 0 || arg0 == 2;
        bool left  = arg0 == 1 || arg0 == 2;
        term_erase(term.esc_mod, true, left, right);
      }
    when 'L':        /* IL: insert lines */
      if (curs->y >= term.marg_top && curs->y <= term.marg_bot
       && curs->x >= term.marg_left && curs->x <= term.marg_right
         )
      {
        term_do_scroll(curs->y, term.marg_bot, -arg0_def1, false);
        curs->x = term.marg_left;
      }
    when 'M':        /* DL: delete lines */
      if (curs->y >= term.marg_top && curs->y <= term.marg_bot
       && curs->x >= term.marg_left && curs->x <= term.marg_right
         )
      {
        term_do_scroll(curs->y, term.marg_bot, arg0_def1, true);
        curs->x = term.marg_left;
      }
    when '@':        /* ICH: insert chars */
      insert_char(arg0_def1);
    when 'P':        /* DCH: delete chars */
      insert_char(-arg0_def1);
    when 'h' or CPAIR('?', 'h'):  /* SM/DECSET: set (private) modes */
      set_modes(true);
    when 'l' or CPAIR('?', 'l'):  /* RM/DECRST: reset (private) modes */
      set_modes(false);
    when CPAIR('?', 's'): { /* Save DEC Private Mode (DECSET) values */
      int arg = term.csi_argv[0];
      int val = get_mode(true, arg);
      if (val)
        push_mode(arg, val);
    }
    when CPAIR('?', 'r'): { /* Restore DEC Private Mode (DECSET) values */
      int arg = term.csi_argv[0];
      int val = pop_mode(arg);
      if (val >= 0) {
        term.csi_argc = 1;
        set_modes(val & 1);
      }
    }
    when CPAIR('#', '{') or CPAIR('#', 'p'): { /* Push video attributes onto stack (XTPUSHSGR) */
      cattr ca = term.curs.attr;
      cattrflags caflagsmask = 0;

      void set_push(int attr) {
        switch (attr) {
          when 1: caflagsmask |= ATTR_BOLD | ATTR_SHADOW;
          when 2: caflagsmask |= ATTR_DIM;
          when 3: caflagsmask |= ATTR_ITALIC;
          when 4 or 21: caflagsmask |= UNDER_MASK;
          when 5 or 6: caflagsmask |= ATTR_BLINK | ATTR_BLINK2;
          when 7: caflagsmask |= ATTR_REVERSE;
          when 8: caflagsmask |= ATTR_INVISIBLE | ATTR_OVERSTRIKE;
          when 9: caflagsmask |= ATTR_STRIKEOUT;
          when 20: caflagsmask |= FONTFAM_MASK;
          when 53: caflagsmask |= ATTR_OVERL;
          when 58: caflagsmask |= ATTR_ULCOLOUR;
          when 30 or 10: caflagsmask |= ATTR_FGMASK;
          when 31 or 11: caflagsmask |= ATTR_BGMASK;
          when 73: caflagsmask |= ATTR_SUPERSCR;
          when 74: caflagsmask |= ATTR_SUBSCR;
        }
      }

      if (!term.csi_argv_defined[0])
        for (int a = 1; a < 90; a++)
          set_push(a);
      else
        for (uint i = 0; i < term.csi_argc; i++) {
          //printf("XTPUSHSGR[%d] %d\n", i, term.csi_argv[i]);
          set_push(term.csi_argv[i]);
        }
      if ((ca.attr & caflagsmask & ATTR_FGMASK) != TRUE_COLOUR)
        ca.truefg = 0;
      if ((ca.attr & caflagsmask & ATTR_BGMASK) != TRUE_COLOUR << ATTR_BGSHIFT)
        ca.truebg = 0;
      if (!(caflagsmask & ATTR_ULCOLOUR))
        ca.ulcolr = (colour)-1;
      // push
      //printf("XTPUSHSGR &%llX %llX %06X %06X %06X\n", caflagsmask, ca.attr, ca.truefg, ca.truebg, ca.ulcolr);
      push_attrs(ca, caflagsmask);
    }
    when CPAIR('#', '}') or CPAIR('#', 'q'): { /* Pop video attributes from stack (XTPOPSGR) */
      //printf("XTPOPSGR\n");
      // pop
      cattr ca;
      cattrflags caflagsmask;
      if (pop_attrs(&ca, &caflagsmask)) {
        //printf("XTPOPSGR &%llX %llX %06X %06X %06X\n", caflagsmask, ca.attr, ca.truefg, ca.truebg, ca.ulcolr);
        // merge
        term.curs.attr.attr = (term.curs.attr.attr & ~caflagsmask)
                              | (ca.attr & caflagsmask);
        if ((ca.attr & caflagsmask & ATTR_FGMASK) == TRUE_COLOUR)
          term.curs.attr.truefg = ca.truefg;
        if ((ca.attr & caflagsmask & ATTR_BGMASK) == TRUE_COLOUR << ATTR_BGSHIFT)
          term.curs.attr.truebg = ca.truebg;
        if (caflagsmask & ATTR_ULCOLOUR)
          term.curs.attr.ulcolr = ca.ulcolr;
      }
    }
    when CPAIR('#', 'P'):  /* Push dynamic colours onto stack (XTPUSHCOLORS) */
      push_colours(arg0);
    when CPAIR('#', 'Q'):  /* Pop dynamic colours from stack (XTPOPCOLORS) */
      pop_colours(arg0);
      win_invalidate_all(false);  // refresh
    when CPAIR('#', 'R'):  /* Report colours stack entry (XTREPORTCOLORS) */
      child_printf("\e[?%d;%d#Q", colours_cur, colours_num);
    when CPAIR('$', 'p'): { /* DECRQM: request (private) mode */
      int arg = term.csi_argv[0];
      child_printf("\e[%s%u;%u$y",
                   esc_mod0 ? "?" : "",
                   arg,
                   get_mode(esc_mod0, arg));
    }
    when 'i' or CPAIR('?', 'i'):  /* MC: Media copy */
      if (arg0 == 5 && *cfg.printer) {
        term.printing = true;
        term.only_printing = !term.esc_mod;
        term.print_state = 0;
        if (*cfg.printer == '*')
          printer_start_job(printer_get_default());
        else
          printer_start_job(cfg.printer);
      }
      else if (arg0 == 4 && term.printing) {
        // Drop escape sequence from print buffer and finish printing.
        while (term.printbuf[--term.printbuf_pos] != '\e');
        term_print_finish();
      }
      else if (arg0 == 10 && !term.esc_mod) {
        term_export_html(false);
      }
#ifdef support_SVG
      else if (arg0 == 11 && !term.esc_mod) {
        term_export_svg();
      }
#endif
      else if (arg0 == 12 && !term.esc_mod) {
        term_save_image();
      }
      else if (arg0 == 0 && !term.esc_mod) {
        print_screen();
      }
    when 'g':        /* TBC: clear tabs */
      if (!arg0)
        term.tabs[curs->x] = false;
      else if (arg0 == 3) {
        for (int i = 0; i < term.cols; i++)
          term.tabs[i] = false;
        term.newtab = 0;  // don't set new default tabs on resize
      }
    when 'r': {      /* DECSTBM: set scrolling region */
      int top = arg0_def1 - 1;
      int bot = (arg1 ? min(arg1, term.rows) : term.rows) - 1;
      if (bot > top) {
        term.marg_top = top;
        term.marg_bot = bot;
        curs->x = curs->origin ? term.marg_left : 0;
        curs->y = curs->origin ? term.marg_top : 0;
      }
    }
    when 's':
      if (term.lrmargmode) {  /* DECSLRM: set left and right margin */
        int left = arg0_def1 - 1;
        int right = (arg1 ? min(arg1, term.cols) : term.cols) - 1;
        if (right > left) {
          term.marg_left = left;
          term.marg_right = right;
          curs->x = curs->origin ? term.marg_left : 0;
          curs->y = curs->origin ? term.marg_top : 0;
        }
      }
      else           /* SCOSC: save cursor */
        save_cursor();
    when 'u':        /* SCORC: restore cursor */
      restore_cursor();
    when 'm':        /* SGR: set graphics rendition */
      do_sgr();
    when 't':
     /*
      * VT340/VT420 sequence DECSLPP, for setting the height of the window.
      * DEC only allowed values 24/25/36/48/72/144, so dtterm and xterm
      * claimed values below 24 for various window operations, 
      * and also allowed any number of rows from 24 and above to be set.
      */
      if (arg0 >= 24) {  /* DECSLPP: set page size - ie window height */
        if (*cfg.suppress_win && contains(cfg.suppress_win, 24))
          ; // skip suppressed window operation
        else {
          win_set_chars(arg0, term.cols);
          term.selected = false;
        }
      }
      else
        do_winop();
    when 'S':        /* SU: Scroll up */
      term_do_scroll(term.marg_top, term.marg_bot, arg0_def1, true);
      curs->wrapnext = false;
    when 'T':        /* SD: Scroll down */
      /* Avoid clash with unsupported hilight mouse tracking mode sequence */
      if (term.csi_argc <= 1) {
        term_do_scroll(term.marg_top, term.marg_bot, -arg0_def1, true);
        curs->wrapnext = false;
      }
    when CPAIR('*', '|'):     /* DECSNLS */
     /*
      * Set number of lines on screen
      * VT420 uses VGA like hardware and can support any size 
      * in reasonable range (24..49 AIUI) with no default specified.
      */
      win_set_chars(arg0 ?: cfg.rows, term.cols);
      term.selected = false;
    when CPAIR('$', '|'):     /* DECSCPP */
     /*
      * Set number of columns per page
      * Docs imply range is only 80 or 132, but I'll allow any.
      */
      win_set_chars(term.rows, arg0 ?: cfg.cols);
      term.selected = false;
    when 'X': {      /* ECH: write N spaces w/o moving cursor */
      termline *line = term.lines[curs->y];
      int cols = min(line->cols, line->size);
      int n = min(arg0_def1, cols - curs->x);
      if (n > 0) {
        int p = curs->x;
        term_check_boundary(curs->x, curs->y);
        term_check_boundary(curs->x + n, curs->y);
        while (n--)
          line->chars[p++] = term.erase_char;
      }
    }
    when 'x':        /* DECREQTPARM: report terminal characteristics */
      if (arg0 <= 1)
        child_printf("\e[%u;1;1;120;120;1;0x", arg0 + 2);
    when 'Z': {      /* CBT (Cursor Backward Tabulation) */
      int n = arg0_def1;
      while (--n >= 0 && curs->x > 0) {
        do
          curs->x--;
        while (curs->x > 0 && !term.tabs[curs->x]);
      }
      enable_progress();
    }
    when CPAIR('$', 'w'):     /* DECTABSR: tab stop report */
      if (arg0 == 2) {
        child_printf("\eP2$");
        char sep = 'u';
        for (int i = 0; i < term.cols; i++)
          if (term.tabs[i]) {
            child_printf("%c%d", sep, i + 1);
            sep = '/';
          }
        child_printf("\e\\");
      }
    when CPAIR('>', 'm'):     /* xterm: modifier key setting */
      /* only the modifyOtherKeys setting is implemented */
      if (!arg0)
        term.modify_other_keys = 0;
      else if (arg0 == 4)
        term.modify_other_keys = arg1;
    when CPAIR('>', 'p'):     /* xterm: pointerMode */
      if (arg0 == 0)
        term.hide_mouse = false;
      else if (arg0 == 2)
        term.hide_mouse = true;
    when CPAIR('>', 'n'):     /* xterm: modifier key setting */
      /* only the modifyOtherKeys setting is implemented */
      if (arg0 == 4)
        term.modify_other_keys = 0;
    when CPAIR(' ', 'q'):     /* DECSCUSR: set cursor style */
      term.cursor_type = arg0 ? (arg0 - 1) / 2 : -1;
      term.cursor_blinks = arg0 ? arg0 % 2 : -1;
      if (term.cursor_blinks)
        term.cursor_blink_interval = arg1;
      term.cursor_invalid = true;
      term_schedule_cblink();
    when CPAIR('?', 'c'):  /* Cursor size (Linux console) */
      term.cursor_size = arg0;
    when CPAIR('"', 'q'):  /* DECSCA: select character protection attribute */
      switch (arg0) {
        when 0 or 2: term.curs.attr.attr &= ~ATTR_PROTECTED;
        when 1: term.curs.attr.attr |= ATTR_PROTECTED;
      }
    when 'n':        /* DSR: device status report */
      if (arg0 == 6)  // CPR
        child_printf("\e[%d;%dR",
                     curs->y + 1 - (curs->origin ? term.marg_top : 0),
                     curs->x + 1 - (curs->origin ? term.marg_left : 0));
      else if (arg0 == 5)
        child_write("\e[0n", 4);
    when CPAIR('?', 'n'):  /* DSR, DEC specific */
      switch (arg0) {
        when 6:  // DECXCPR
          child_printf("\e[?%d;%dR",  // VT420: third parameter "page"...
                       curs->y + 1 - (curs->origin ? term.marg_top : 0),
                       curs->x + 1 - (curs->origin ? term.marg_left : 0));
        when 15:
          child_printf("\e[?%un", 11 - !!*cfg.printer);
        // DEC Locator
        when 53 or 55:
          child_printf("\e[?53n");
        when 56:
          child_printf("\e[?57;1n");
      }
    // DEC Locator
    when CPAIR('\'', 'z'): {  /* DECELR: enable locator reporting */
      switch (arg0) {
        when 0:
          if (term.mouse_mode == MM_LOCATOR) {
            term.mouse_mode = 0;
            win_update_mouse();
          }
          term.locator_1_enabled = false;
        when 1:
          term.mouse_mode = MM_LOCATOR;
          win_update_mouse();
        when 2:
          term.locator_1_enabled = true;
          win_update_mouse();
      }
      switch (arg1) {
        when 0 or 2:
          term.locator_by_pixels = false;
        when 1:
          term.locator_by_pixels = true;
      }
      term.locator_rectangle = false;
    }
    when CPAIR('\'', '{'): {  /* DECSLE: select locator events */
      for (uint i = 0; i < term.csi_argc; i++)
        switch (term.csi_argv[i]) {
          when 0: term.locator_report_up = term.locator_report_dn = false;
          when 1: term.locator_report_dn = true;
          when 2: term.locator_report_dn = false;
          when 3: term.locator_report_up = true;
          when 4: term.locator_report_up = false;
        }
    }
    when CPAIR('\'', '|'): {  /* DECRQLP: request locator position */
      if (term.mouse_mode == MM_LOCATOR || term.locator_1_enabled) {
        int x, y, buttons;
        win_get_locator_info(&x, &y, &buttons, term.locator_by_pixels);
        child_printf("\e[1;%d;%d;%d;0&w", buttons, y, x);
        term.locator_1_enabled = false;
      }
      else {
        //child_printf("\e[0&w");  // xterm reports this if loc. compiled in
      }
    }
    when CPAIR('\'', 'w'): {  /* DECEFR: enable filter rectangle */
      int arg2 = term.csi_argv[2], arg3 = term.csi_argv[3];
      int x, y, buttons;
      win_get_locator_info(&x, &y, &buttons, term.locator_by_pixels);
      term.locator_top = arg0 ?: y;
      term.locator_left = arg1 ?: x;
      term.locator_bottom = arg2 ?: y;
      term.locator_right = arg3 ?: x;
      term.locator_rectangle = true;
    }
    when 'q': {  /* DECLL: load keyboard LEDs */
      if (arg0 > 20)
        win_led(arg0 - 20, false);
      else if (arg0)
        win_led(arg0, true);
      else {
        win_led(0, false);
      }
    }
    when CPAIR(' ', 'k'):  /* SCP: ECMA-48 Set Character Path (LTR/RTL) */
      if (arg0 <= 2) {
        if (arg0 == 2)
          curs->bidimode |= LATTR_BIDIRTL;
        else if (arg0 == 1)
          curs->bidimode &= ~LATTR_BIDIRTL;
        else {  // default
          curs->bidimode &= ~(LATTR_BIDISEL | LATTR_BIDIRTL);
        }
        // postpone propagation to line until char is written (put_char)
        //termline *line = term.lines[curs->y];
        //line->lattr &= ~(LATTR_BIDISEL | LATTR_BIDIRTL);
        //line->lattr |= curs->bidimode & ~LATTR_BIDISEL | LATTR_BIDIRTL);
      }
    when CPAIR(' ', 'S'):  /* SPD: ECMA-48 Select Presentation Direction */
      if (arg0 == 0)
          curs->bidimode &= ~LATTR_PRESRTL;
      else if (arg0 == 3)
          curs->bidimode |= LATTR_PRESRTL;
#define urows (uint) term.rows
#define ucols (uint) term.cols
    when CPAIR('$', 'v'):  /* DECCRA: VT420 Copy Rectangular Area */
      copy_rect(arg0_def1, arg1 ?: 1, 
                term.csi_argv[2] ?: urows, term.csi_argv[3] ?: ucols,
                // skip term.csi_argv[4] (source page)
                term.csi_argv[5] ?: urows, term.csi_argv[6] ?: ucols
                // skip term.csi_argv[7] (destination page)
                );
    when CPAIR('$', 'x'):  /* DECFRA: VT420 Fill Rectangular Area */
      fill_rect(arg0 ?: ' ', curs->attr, false,
                arg1 ?: 1, term.csi_argv[2] ?: 1,
                term.csi_argv[3] ?: urows, term.csi_argv[4] ?: ucols);
    when CPAIR('$', 'z'):  /* DECERA: VT420 Erase Rectangular Area */
      fill_rect(' ', term.erase_char.attr, false,
                arg0_def1, arg1 ?: 1,
                term.csi_argv[2] ?: urows, term.csi_argv[3] ?: ucols);
    when CPAIR('$', '{'):  /* DECSERA: VT420 Selective Erase Rectangular Area */
      fill_rect(' ', term.erase_char.attr, true,
                arg0_def1, arg1 ?: 1,
                term.csi_argv[2] ?: urows, term.csi_argv[3] ?: ucols);
    when CPAIR('*', 'x'):  /* DECSACE: VT420 Select Attribute Change Extent */
      switch (arg0) {
        when 2: term.attr_rect = true;
        when 0 or 1: term.attr_rect = false;
      }
    when CPAIR('$', 'r')  /* DECCARA: VT420 Change Attributes in Area */
      or CPAIR('$', 't'): {  /* DECRARA: VT420 Reverse Attributes in Area */
      cattrflags a1 = 0, a2 = 0;
      for (uint i = 4; i < term.csi_argc; i++)
        switch (term.csi_argv[i]) {
          when 0: a2 = ATTR_BOLD | ATTR_UNDER | ATTR_BLINK | ATTR_REVERSE;
          when 1: a1 |= ATTR_BOLD;
          when 4: a1 |= ATTR_UNDER;
          when 5: a1 |= ATTR_BLINK;
          when 7: a1 |= ATTR_REVERSE;
          when 22: a2 |= ATTR_BOLD;
          when 24: a2 |= ATTR_UNDER;
          when 25: a2 |= ATTR_BLINK;
          when 27: a2 |= ATTR_REVERSE;
          //when 2: a1 |= ATTR_DIM;
          //when 3: a1 |= ATTR_ITALIC;
          //when 6: a1 |= ATTR_BLINK2;
          //when 8: a1 |= ATTR_INVISIBLE;
          //when 9: a1 |= ATTR_STRIKEOUT;
        }
      a1 &= ~a2;
      if (c == 'r')
        attr_rect(a1, a2, 0, arg0_def1, arg1 ?: 1,
                  term.csi_argv[2] ?: urows, term.csi_argv[3] ?: ucols);
      else
        attr_rect(0, 0, a1, arg0_def1, arg1 ?: 1,
                  term.csi_argv[2] ?: urows, term.csi_argv[3] ?: ucols);
    }
    when CPAIR('*', 'y'): { /* DECRQCRA: VT420 Request Rectangular Checksum */
      uint s = sum_rect(term.csi_argv[2] ?: 1, term.csi_argv[3] ?: 1,
                        term.csi_argv[4] ?: urows, term.csi_argv[5] ?: ucols);
      child_printf("\eP%u!~%04X\e\\", arg0, -s & 0xFFFF);
    }
    when CPAIR('\'', '}'):  /* DECIC: VT420 Insert Columns */
      if (curs->x >= term.marg_left && curs->x <= term.marg_right
       && curs->y >= term.marg_top && curs->y <= term.marg_bot
         )
        insdel_column(curs->x, false, arg0_def1);
    when CPAIR('\'', '~'):  /* DECDC: VT420 Delete Columns */
      if (curs->x >= term.marg_left && curs->x <= term.marg_right
       && curs->y >= term.marg_top && curs->y <= term.marg_bot
         )
        insdel_column(curs->x, true, arg0_def1);
    when CPAIR(' ', 'A'):     /* SR: ECMA-48 shift columns right */
      if (curs->x >= term.marg_left && curs->x <= term.marg_right
       && curs->y >= term.marg_top && curs->y <= term.marg_bot
         )
        insdel_column(term.marg_left, false, arg0_def1);
    when CPAIR(' ', '@'):     /* SR: ECMA-48 shift columns left */
      if (curs->x >= term.marg_left && curs->x <= term.marg_right
       && curs->y >= term.marg_top && curs->y <= term.marg_bot
         )
        insdel_column(term.marg_left, true, arg0_def1);
    when CPAIR('#', 't'):  /* application scrollbar */
      win_set_scrollview(arg0, arg1, term.csi_argc > 2 ? (int)term.csi_argv[2] : -1);
    when CPAIR('<', 't'):  /* TTIMEST: change IME state (Tera Term) */
      win_set_ime(arg0);
    when CPAIR('<', 's'):  /* TTIMESV: save IME state (Tera Term) */
      push_mode(-1, win_get_ime());
    when CPAIR('<', 'r'):  /* TTIMERS: restore IME state (Tera Term) */
      win_set_ime(pop_mode(-1));
    when CPAIR(' ', 't'):     /* DECSWBV: VT520 warning bell volume */
      if (arg0 <= 8)
        term.bell.vol = arg0;
    when CPAIR(' ', 'u'):     /* DECSMBV: VT520 margin bell volume */
      if (!arg0)
        term.marginbell.vol = 8;
      else if (arg0 <= 8)
        term.marginbell.vol = arg0;
    when CPAIR(' ', 'Z'): /* PEC: ECMA-48 Presentation Expand Or Contract */
      if (!arg0)
        curs->width = 0;
      else if (arg0 == 1)   // expanded
        curs->width = 2;
      else if (arg0 == 2) { // condensed
        if (arg1 == 2)      // single-cell zoomed down
          curs->width = 11;
        else
          curs->width = 1;
      }
      else if (arg0 == 22)  // single-cell zoomed down
        curs->width = 11;
#ifdef support_triple_width
      else if (arg0 == 3)   // triple-cell
        curs->width = 3;
#endif
    when CPAIR('-', 'p'): /* DECARR: VT520 Select Auto Repeat Rate */
      if (arg0 <= 30)
        term.repeat_rate = arg0;
    when CPAIR('%', 'q'):  /* setup progress indicator on taskbar icon */
      set_taskbar_progress(arg0, term.csi_argc > 1 ? arg1 : -1);
    when 'y':  /* DECTST */
      if (arg0 == 4) {
        cattr attr = (cattr)
                     {.attr = ATTR_DEFFG | (TRUE_COLOUR << ATTR_BGSHIFT),
                      .truefg = 0, .truebg = 0, .ulcolr = (colour)-1,
                      .link = -1
                     };
        switch (arg1) {
          when 10: attr.truebg = RGB(0, 0, 255);
          when 11: attr.truebg = RGB(255, 0, 0);
          when 12: attr.truebg = RGB(0, 255, 0);
          when 13: attr.truebg = RGB(255, 255, 255);
          otherwise: return;
        }
        for (int i = 0; i < term.rows; i++) {
          termline *line = term.lines[i];
          for (int j = 0; j < term.cols; j++) {
            line->chars[j] =
              (termchar) {.cc_next = 0, .chr = ' ', attr};
          }
          line->lattr = LATTR_NORM;
        }
        term.disptop = 0;
      }
  }
}