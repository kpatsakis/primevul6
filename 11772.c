PutAttrChar(NCURSES_SP_DCLx CARG_CH_T ch)
{
    int chlen = 1;
    NCURSES_CH_T my_ch;
#if USE_WIDEC_SUPPORT
    PUTC_DATA;
#endif
    NCURSES_CH_T tilde;
    NCURSES_CH_T attr = CHDEREF(ch);

    TR(TRACE_CHARPUT, ("PutAttrChar(%s) at (%d, %d)",
		       _tracech_t(ch),
		       SP_PARM->_cursrow, SP_PARM->_curscol));
#if USE_WIDEC_SUPPORT
    /*
     * If this is not a valid character, there is nothing more to do.
     */
    if (isWidecExt(CHDEREF(ch))) {
	TR(TRACE_CHARPUT, ("...skip"));
	return;
    }
    /*
     * Determine the number of character cells which the 'ch' value will use
     * on the screen.  It should be at least one.
     */
    if ((chlen = _nc_wacs_width(CharOf(CHDEREF(ch)))) <= 0) {
	static const NCURSES_CH_T blank = NewChar(BLANK_TEXT);

	/*
	 * If the character falls into any of these special cases, do
	 * not force the result to a blank:
	 *
	 * a) it is printable (this works around a bug in wcwidth()).
	 * b) use_legacy_coding() has been called to modify the treatment
	 *    of codes 128-255.
	 * c) the acs_map[] has been initialized to allow codes 0-31
	 *    to be rendered.  This supports Linux console's "PC"
	 *    characters.  Codes 128-255 are allowed though this is
	 *    not checked.
	 */
	if (is8bits(CharOf(CHDEREF(ch)))
	    && (isprint(CharOf(CHDEREF(ch)))
		|| (SP_PARM->_legacy_coding > 0 && CharOf(CHDEREF(ch)) >= 160)
		|| (SP_PARM->_legacy_coding > 1 && CharOf(CHDEREF(ch)) >= 128)
		|| (AttrOf(attr) & A_ALTCHARSET
		    && ((CharOfD(ch) < ACS_LEN
			 && SP_PARM->_acs_map != 0
			 && SP_PARM->_acs_map[CharOfD(ch)] != 0)
			|| (CharOfD(ch) >= 128))))) {
	    ;
	} else {
	    ch = CHREF(blank);
	    TR(TRACE_CHARPUT, ("forced to blank"));
	}
	chlen = 1;
    }
#endif

    if ((AttrOf(attr) & A_ALTCHARSET)
	&& SP_PARM->_acs_map != 0
	&& ((CharOfD(ch) < ACS_LEN)
#if !NCURSES_WCWIDTH_GRAPHICS
	    || is_wacs_value(CharOfD(ch))
#endif
	)) {
	int c8;
	my_ch = CHDEREF(ch);	/* work around const param */
	c8 = CharOf(my_ch);
#if USE_WIDEC_SUPPORT
	/*
	 * This is crude & ugly, but works most of the time.  It checks if the
	 * acs_chars string specified that we have a mapping for this
	 * character, and uses the wide-character mapping when we expect the
	 * normal one to be broken (by mis-design ;-).
	 */
	if (SP_PARM->_screen_unicode
	    && _nc_wacs[CharOf(my_ch)].chars[0]) {
	    if (SP_PARM->_screen_acs_map[CharOf(my_ch)]) {
		if (SP_PARM->_screen_acs_fix) {
		    RemAttr(attr, A_ALTCHARSET);
		    my_ch = _nc_wacs[CharOf(my_ch)];
		}
	    } else {
		RemAttr(attr, A_ALTCHARSET);
		my_ch = _nc_wacs[CharOf(my_ch)];
	    }
#if !NCURSES_WCWIDTH_GRAPHICS
	    if (!(AttrOf(attr) & A_ALTCHARSET)) {
		chlen = 1;
	    }
#endif /* !NCURSES_WCWIDTH_GRAPHICS */
	} else
#endif
	if (!SP_PARM->_screen_acs_map[c8]) {
	    /*
	     * If we found no mapping for a given alternate-character set item
	     * in the terminal description, attempt to use the ASCII fallback
	     * code which is populated in the _acs_map[] array.  If that did
	     * not correspond to a line-drawing, etc., graphics character, the
	     * array entry would be empty.
	     */
	    chtype temp = UChar(SP_PARM->_acs_map[c8]);
	    if (temp) {
		RemAttr(attr, A_ALTCHARSET);
		SetChar(my_ch, temp, AttrOf(attr));
	    }
	}

	/*
	 * If we (still) have alternate character set, it is the normal 8bit
	 * flavor.  The _screen_acs_map[] array tells if the character was
	 * really in acs_chars, needed because of the way wide/normal line
	 * drawing flavors are integrated.
	 */
	if (AttrOf(attr) & A_ALTCHARSET) {
	    int j = CharOfD(ch);
	    chtype temp = UChar(SP_PARM->_acs_map[j]);

	    if (temp != 0) {
		SetChar(my_ch, temp, AttrOf(attr));
	    } else {
		my_ch = CHDEREF(ch);
		RemAttr(attr, A_ALTCHARSET);
	    }
	}
	ch = CHREF(my_ch);
    }
#if USE_WIDEC_SUPPORT && !NCURSES_WCWIDTH_GRAPHICS
    else if (chlen > 1 && is_wacs_value(CharOfD(ch))) {
	chlen = 1;
    }
#endif
    if (tilde_glitch && (CharOfD(ch) == L('~'))) {
	SetChar(tilde, L('`'), AttrOf(attr));
	ch = CHREF(tilde);
    }

    UpdateAttrs(SP_PARM, attr);
    PUTC(CHDEREF(ch));
#if !USE_WIDEC_SUPPORT
    COUNT_OUTCHARS(1);
#endif
    SP_PARM->_curscol += chlen;
    if (char_padding) {
	NCURSES_PUTP2("char_padding", char_padding);
    }
}