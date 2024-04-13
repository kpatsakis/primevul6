set_modes(bool state)
{
  for (uint i = 0; i < term.csi_argc; i++) {
    uint arg = term.csi_argv[i];
    if (term.esc_mod) { /* DECSET/DECRST: DEC private mode set/reset */
      if (*cfg.suppress_dec && contains(cfg.suppress_dec, arg))
        ; // skip suppressed DECSET/DECRST operation
      else
      switch (arg) {
        when 1:  /* DECCKM: application cursor keys */
          term.app_cursor_keys = state;
        when 66:  /* DECNKM: application keypad */
          term.app_keypad = state;
        when 2:  /* DECANM: VT100/VT52 mode */
          if (state) {
            // Designate USASCII for character sets G0-G3
            for (uint i = 0; i < lengthof(term.curs.csets); i++)
              term.curs.csets[i] = CSET_ASCII;
            term.curs.cset_single = CSET_ASCII;
            term_update_cs();
          }
          else
            term.vt52_mode = 1;
        when 3:  /* DECCOLM: 80/132 columns */
          if (term.deccolm_allowed) {
            term.selected = false;
            win_set_chars(term.rows, state ? 132 : 80);
            term.reset_132 = state;
            term.marg_top = 0;
            term.marg_bot = term.rows - 1;
            term.marg_left = 0;
            term.marg_right = term.cols - 1;
            move(0, 0, 0);
            if (!term.deccolm_noclear)
              term_erase(false, false, true, true);
          }
        when 5:  /* DECSCNM: reverse video */
          if (state != term.rvideo) {
            term.rvideo = state;
            win_invalidate_all(false);
          }
        when 6:  /* DECOM: DEC origin mode */
          term.curs.origin = state;
          if (state)
            move(term.marg_left, term.marg_top, 0);
          else
            move(0, 0, 0);
        when 7:  /* DECAWM: auto wrap */
          term.autowrap = state;
          term.curs.wrapnext = false;
        when 45:  /* xterm: reverse (auto) wraparound */
          term.rev_wrap = state;
          term.curs.wrapnext = false;
        when 8:  /* DECARM: auto key repeat */
          term.auto_repeat = state;
        when 9:  /* X10_MOUSE */
          term.mouse_mode = state ? MM_X10 : 0;
          win_update_mouse();
        when 12: /* AT&T 610 blinking cursor */
          term.cursor_blinkmode = state;
          term.cursor_invalid = true;
          term_schedule_cblink();
        when 25: /* DECTCEM: enable/disable cursor */
          term.cursor_on = state;
          // Should we set term.cursor_invalid or call term_invalidate ?
        when 30: /* Show/hide scrollbar */
          if (state != term.show_scrollbar) {
            term.show_scrollbar = state;
            win_update_scrollbar(false);
          }
        when 38: /* DECTEK: Enter Tektronix Mode (VT240, VT330) */
          if (state) {
            tek_mode = TEKMODE_ALPHA;
            tek_init(true, cfg.tek_glow);
          }
        when 40: /* Allow/disallow DECCOLM (xterm c132 resource) */
          term.deccolm_allowed = state;
        when 95: /* VT510 DECNCSM: DECCOLM does not clear the screen */
          term.deccolm_noclear = state;
        when 42: /* DECNRCM: national replacement character sets */
          term.decnrc_enabled = state;
        when 44: /* turn on margin bell (xterm) */
          term.margin_bell = state;
        when 67: /* DECBKM: backarrow key mode */
          term.backspace_sends_bs = state;
        when 69: /* DECLRMM/VT420 DECVSSM: enable left/right margins DECSLRM */
          term.lrmargmode = state;
          if (state) {
            for (int i = 0; i < term.rows; i++) {
              termline *line = term.lines[i];
              line->lattr = LATTR_NORM;
            }
          }
          else {
            term.marg_left = 0;
            term.marg_right = term.cols - 1;
          }
        when 80: /* DECSDM: SIXEL display mode */
          term.sixel_display = !state;
        when 1000: /* VT200_MOUSE */
          term.mouse_mode = state ? MM_VT200 : 0;
          win_update_mouse();
        when 1002: /* BTN_EVENT_MOUSE */
          term.mouse_mode = state ? MM_BTN_EVENT : 0;
          win_update_mouse();
        when 1003: /* ANY_EVENT_MOUSE */
          term.mouse_mode = state ? MM_ANY_EVENT : 0;
          win_update_mouse();
        when 1004: /* FOCUS_EVENT_MOUSE */
          term.report_focus = state;
        when 1005: /* Xterm's UTF8 encoding for mouse positions */
          term.mouse_enc = state ? ME_UTF8 : 0;
        when 1006: /* Xterm's CSI-style mouse encoding */
          term.mouse_enc = state ? ME_XTERM_CSI : 0;
        when 1016: /* Xterm's CSI-style mouse encoding with pixel resolution */
          term.mouse_enc = state ? ME_PIXEL_CSI : 0;
        when 1015: /* Urxvt's CSI-style mouse encoding */
          term.mouse_enc = state ? ME_URXVT_CSI : 0;
        when 1037:
          term.delete_sends_del = state;
        when 1042:
          term.bell_taskbar = state;
        when 1043:
          term.bell_popup = state;
        when 47: /* alternate screen */
          if (!cfg.disable_alternate_screen) {
            term.selected = false;
            term_switch_screen(state, false);
            term.disptop = 0;
          }
        when 1047:       /* alternate screen */
          if (!cfg.disable_alternate_screen) {
            term.selected = false;
            term_switch_screen(state, true);
            term.disptop = 0;
          }
        when 1046:       /* enable/disable alternate screen switching */
          if (term.on_alt_screen && !state)
            term_switch_screen(false, false);
          cfg.disable_alternate_screen = !state;
        when 1048:       /* save/restore cursor */
          if (!cfg.disable_alternate_screen) {
            if (state)
              save_cursor();
            else
              restore_cursor();
          }
        when 1049:       /* cursor & alternate screen */
          if (!cfg.disable_alternate_screen) {
            if (state)
              save_cursor();
            term.selected = false;
            term_switch_screen(state, true);
            if (!state)
              restore_cursor();
            term.disptop = 0;
          }
        when 1061:       /* VT220 keyboard emulation */
          term.vt220_keys = state;
        when 2004:       /* xterm bracketed paste mode */
          term.bracketed_paste = state;

        /* Mintty private modes */
        when 7700:       /* CJK ambigous width reporting */
          term.report_ambig_width = state;
        when 7711:       /* Scroll marker in current line */
          if (state)
            term.lines[term.curs.y]->lattr |= LATTR_MARKED;
          else
            term.lines[term.curs.y]->lattr |= LATTR_UNMARKED;
        when 7727:       /* Application escape key mode */
          term.app_escape_key = state;
        when 7728:       /* Escape sends FS (instead of ESC) */
          term.escape_sends_fs = state;
        when 7730:       /* Sixel scrolling end position */
          /* on: sixel scrolling moves cursor to beginning of the line
             off(default): sixel scrolling moves cursor to left of graphics */
          term.sixel_scrolls_left = state;
        when 7766:       /* 'B': Show/hide scrollbar (if enabled in config) */
          if (cfg.scrollbar && state != term.show_scrollbar) {
            term.show_scrollbar = state;
            win_update_scrollbar(true);
          }
        when 7767:       /* 'C': Changed font reporting */
          term.report_font_changed = state;
        when 7783:       /* 'S': Shortcut override */
          term.shortcut_override = state;
        when 1007:       /* Alternate Scroll Mode, xterm */
          term.wheel_reporting_xterm = state;
        when 7786:       /* 'V': Mousewheel reporting */
          term.wheel_reporting = state;
        when 7787:       /* 'W': Application mousewheel mode */
          term.app_wheel = state;
        when 7796:       /* Bidi disable in current line */
          if (state)
            term.lines[term.curs.y]->lattr |= LATTR_NOBIDI;
          else
            term.lines[term.curs.y]->lattr &= ~LATTR_NOBIDI;
        when 77096:      /* Bidi disable */
          term.disable_bidi = state;
        when 8452:       /* Sixel scrolling end position right */
          /* on: sixel scrolling leaves cursor to right of graphic
             off(default): position after sixel depends on sixel_scrolls_left */
          term.sixel_scrolls_right = state;
        when 77000 ... 77031: { /* Application control key modes */
          int ctrl = arg - 77000;
          term.app_control = (term.app_control & ~(1 << ctrl)) | (state << ctrl);
        }
        when 2500: /* bidi box graphics mirroring */
          if (state)
            term.curs.bidimode |= LATTR_BOXMIRROR;
          else
            term.curs.bidimode &= ~LATTR_BOXMIRROR;
        when 2501: /* bidi direction auto-detection */
          if (state)
            term.curs.bidimode &= ~LATTR_BIDISEL;
          else
            term.curs.bidimode |= LATTR_BIDISEL;
      }
    }
    else { /* SM/RM: set/reset mode */
      switch (arg) {
        when 4:  /* IRM: set insert mode */
          term.insert = state;
        when 8: /* BDSM: ECMA-48 bidirectional support mode */
          if (state)
            term.curs.bidimode &= ~LATTR_NOBIDI;
          else
            term.curs.bidimode |= LATTR_NOBIDI;
        when 12: /* SRM: set echo mode */
          term.echoing = !state;
        when 20: /* LNM: Return sends ... */
          term.newline_mode = state;
#ifdef support_Wyse_cursor_modes
        when 33: /* WYSTCURM: steady Wyse cursor */
          term.cursor_blinkmode = !state;
          term.cursor_invalid = true;
          term_schedule_cblink();
        when 34: /* WYULCURM: Wyse underline cursor */
          term.cursor_type = state;
          term.cursor_blinkmode = false;
          term.cursor_invalid = true;
          term_schedule_cblink();
#endif
      }
    }
  }
}