_nc_cookie_init(SCREEN *sp)
{
    bool support_cookies = USE_XMC_SUPPORT;
    TERMINAL_CONTROL_BLOCK *TCB = (TERMINAL_CONTROL_BLOCK *) (sp->_term);

    if (sp == 0 || !ENSURE_TINFO(sp))
	return;

#if USE_XMC_SUPPORT
    /*
     * If we have no magic-cookie support compiled-in, or if it is suppressed
     * in the environment, reset the support-flag.
     */
    if (magic_cookie_glitch >= 0) {
	if (getenv("NCURSES_NO_MAGIC_COOKIE") != 0) {
	    support_cookies = FALSE;
	}
    }
#endif

    if (!support_cookies && magic_cookie_glitch >= 0) {
	T(("will disable attributes to work w/o magic cookies"));
    }

    if (magic_cookie_glitch > 0) {	/* tvi, wyse */

	sp->_xmc_triggers = sp->_ok_attributes & XMC_CONFLICT;
#if 0
	/*
	 * We "should" treat colors as an attribute.  The wyse350 (and its
	 * clones) appear to be the only ones that have both colors and magic
	 * cookies.
	 */
	if (has_colors()) {
	    sp->_xmc_triggers |= A_COLOR;
	}
#endif
	sp->_xmc_suppress = sp->_xmc_triggers & (chtype) ~(A_BOLD);

	T(("magic cookie attributes %s", _traceattr(sp->_xmc_suppress)));
	/*
	 * Supporting line-drawing may be possible.  But make the regular
	 * video attributes work first.
	 */
	acs_chars = ABSENT_STRING;
	ena_acs = ABSENT_STRING;
	enter_alt_charset_mode = ABSENT_STRING;
	exit_alt_charset_mode = ABSENT_STRING;
#if USE_XMC_SUPPORT
	/*
	 * To keep the cookie support simple, suppress all of the optimization
	 * hooks except for clear_screen and the cursor addressing.
	 */
	if (support_cookies) {
	    clr_eol = ABSENT_STRING;
	    clr_eos = ABSENT_STRING;
	    set_attributes = ABSENT_STRING;
	}
#endif
    } else if (magic_cookie_glitch == 0) {	/* hpterm */
    }

    /*
     * If magic cookies are not supported, cancel the strings that set
     * video attributes.
     */
    if (!support_cookies && magic_cookie_glitch >= 0) {
	magic_cookie_glitch = ABSENT_NUMERIC;
	set_attributes = ABSENT_STRING;
	enter_blink_mode = ABSENT_STRING;
	enter_bold_mode = ABSENT_STRING;
	enter_dim_mode = ABSENT_STRING;
	enter_reverse_mode = ABSENT_STRING;
	enter_standout_mode = ABSENT_STRING;
	enter_underline_mode = ABSENT_STRING;
    }

    /* initialize normal acs before wide, since we use mapping in the latter */
#if !USE_WIDEC_SUPPORT
    if (_nc_unicode_locale() && _nc_locale_breaks_acs(sp->_term)) {
	acs_chars = NULL;
	ena_acs = NULL;
	enter_alt_charset_mode = NULL;
	exit_alt_charset_mode = NULL;
	set_attributes = NULL;
    }
#endif
}