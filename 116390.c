get_mode(bool privatemode, int arg)
{
  if (privatemode) { /* DECRQM for DECSET/DECRST: DEC private mode */
    switch (arg) {
      when 1:  /* DECCKM: application cursor keys */
        return 2 - term.app_cursor_keys;
      when 66:  /* DECNKM: application keypad */
        return 2 - term.app_keypad;
      when 2:  /* DECANM: VT100/VT52 mode */
        // Check USASCII for character sets G0-G3
        for (uint i = 0; i < lengthof(term.curs.csets); i++)
          if (term.curs.csets[i] != CSET_ASCII)
            return 2;
        return 1;
      when 3:  /* DECCOLM: 80/132 columns */
        return 2 - term.reset_132;
      when 5:  /* DECSCNM: reverse video */
        return 2 - term.rvideo;
      when 6:  /* DECOM: DEC origin mode */
        return 2 - term.curs.origin;
      when 7:  /* DECAWM: auto wrap */
        return 2 - term.autowrap;
      when 45:  /* xterm: reverse (auto) wraparound */
        return 2 - term.rev_wrap;
      when 8:  /* DECARM: auto key repeat */
        return 2 - term.auto_repeat;
        //return 3; // ignored
      when 9:  /* X10_MOUSE */
        return 2 - (term.mouse_mode == MM_X10);
      when 12: /* AT&T 610 blinking cursor */
        return 2 - term.cursor_blinkmode;
      when 25: /* DECTCEM: enable/disable cursor */
        return 2 - term.cursor_on;
      when 30: /* Show/hide scrollbar */
        return 2 - term.show_scrollbar;
      when 40: /* Allow/disallow DECCOLM (xterm c132 resource) */
        return 2 - term.deccolm_allowed;
      when 42: /* DECNRCM: national replacement character sets */
        return 2 - term.decnrc_enabled;
      when 44: /* margin bell (xterm) */
        return 2 - term.margin_bell;
      when 67: /* DECBKM: backarrow key mode */
        return 2 - term.backspace_sends_bs;
      when 69: /* DECLRMM: enable left and right margin mode DECSLRM */
        return 2 - term.lrmargmode;
      when 80: /* DECSDM: SIXEL display mode */
        return 2 - !term.sixel_display;
      when 1000: /* VT200_MOUSE */
        return 2 - (term.mouse_mode == MM_VT200);
      when 1002: /* BTN_EVENT_MOUSE */
        return 2 - (term.mouse_mode == MM_BTN_EVENT);
      when 1003: /* ANY_EVENT_MOUSE */
        return 2 - (term.mouse_mode == MM_ANY_EVENT);
      when 1004: /* FOCUS_EVENT_MOUSE */
        return 2 - term.report_focus;
      when 1005: /* Xterm's UTF8 encoding for mouse positions */
        return 2 - (term.mouse_enc == ME_UTF8);
      when 1006: /* Xterm's CSI-style mouse encoding */
        return 2 - (term.mouse_enc == ME_XTERM_CSI);
      when 1016: /* Xterm's CSI-style mouse encoding with pixel resolution */
        return 2 - (term.mouse_enc == ME_PIXEL_CSI);
      when 1015: /* Urxvt's CSI-style mouse encoding */
        return 2 - (term.mouse_enc == ME_URXVT_CSI);
      when 1037:
        return 2 - term.delete_sends_del;
      when 1042:
        return 2 - term.bell_taskbar;
      when 1043:
        return 2 - term.bell_popup;
      when 47: /* alternate screen */
        return 2 - term.on_alt_screen;
      when 1047:       /* alternate screen */
        return 2 - term.on_alt_screen;
      when 1048:       /* save/restore cursor */
        return 4;
      when 1049:       /* cursor & alternate screen */
        return 2 - term.on_alt_screen;
      when 1061:       /* VT220 keyboard emulation */
        return 2 - term.vt220_keys;
      when 2004:       /* xterm bracketed paste mode */
        return 2 - term.bracketed_paste;

      /* Mintty private modes */
      when 7700:       /* CJK ambigous width reporting */
        return 2 - term.report_ambig_width;
      when 7711:       /* Scroll marker in current line */
        return 2 - !!(term.lines[term.curs.y]->lattr & LATTR_MARKED);
      when 7727:       /* Application escape key mode */
        return 2 - term.app_escape_key;
      when 7728:       /* Escape sends FS (instead of ESC) */
        return 2 - term.escape_sends_fs;
      when 7730:       /* Sixel scrolling end position */
        return 2 - term.sixel_scrolls_left;
      when 7766:       /* 'B': Show/hide scrollbar (if enabled in config) */
        return 2 - term.show_scrollbar;
      when 7767:       /* 'C': Changed font reporting */
        return 2 - term.report_font_changed;
      when 7783:       /* 'S': Shortcut override */
        return 2 - term.shortcut_override;
      when 1007:       /* Alternate Scroll Mode, xterm */
        return 2 - term.wheel_reporting_xterm;
      when 7786:       /* 'V': Mousewheel reporting */
        return 2 - term.wheel_reporting;
      when 7787:       /* 'W': Application mousewheel mode */
        return 2 - term.app_wheel;
      when 7796:       /* Bidi disable in current line */
        return 2 - !!(term.lines[term.curs.y]->lattr & LATTR_NOBIDI);
      when 77096:      /* Bidi disable */
        return 2 - term.disable_bidi;
      when 8452:       /* Sixel scrolling end position right */
        return 2 - term.sixel_scrolls_right;
      when 77000 ... 77031: { /* Application control key modes */
        int ctrl = arg - 77000;
        return 2 - !!(term.app_control & (1 << ctrl));
      }
      when 2500: /* bidi box graphics mirroring */
        return 2 - !!(term.curs.bidimode & LATTR_BOXMIRROR);
      when 2501: /* bidi direction auto-detection */
        return 2 - !(term.curs.bidimode & LATTR_BIDISEL);
      otherwise:
        return 0;
    }
  }
  else { /* DECRQM for SM/RM: mode */
    switch (arg) {
      when 4:  /* IRM: insert mode */
        return 2 - term.insert;
      when 8: /* BDSM: bidirectional support mode */
        return 2 - !(term.curs.bidimode & LATTR_NOBIDI);
      when 12: /* SRM: echo mode */
        return 2 - !term.echoing;
      when 20: /* LNM: Return sends ... */
        return 2 - term.newline_mode;
#ifdef support_Wyse_cursor_modes
      when 33: /* WYSTCURM: steady Wyse cursor */
        return 2 - (!term.cursor_blinkmode);
      when 34: /* WYULCURM: Wyse underline cursor */
        if (term.cursor_type <= 1)
          return 2 - (term.cursor_type == 1);
        else
          return 0;
#endif
      otherwise:
        return 0;
    }
  }
}