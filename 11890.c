drv_initacs(TERMINAL_CONTROL_BLOCK * TCB, chtype *real_map, chtype *fake_map)
{
    SCREEN *sp = TCB->csp;

    AssertTCB();
    assert(sp != 0);
    if (ena_acs != NULL) {
	NCURSES_PUTP2("ena_acs", ena_acs);
    }
#if NCURSES_EXT_FUNCS
    /*
     * Linux console "supports" the "PC ROM" character set by the coincidence
     * that smpch/rmpch and smacs/rmacs have the same values.  ncurses has
     * no codepage support (see SCO Merge for an example).  Outside of the
     * values defined in acsc, there are no definitions for the "PC ROM"
     * character set (assumed by some applications to be codepage 437), but we
     * allow those applications to use those codepoints.
     *
     * test/blue.c uses this feature.
     */
#define PCH_KLUDGE(a,b) (a != 0 && b != 0 && !strcmp(a,b))
    if (PCH_KLUDGE(enter_pc_charset_mode, enter_alt_charset_mode) &&
	PCH_KLUDGE(exit_pc_charset_mode, exit_alt_charset_mode)) {
	size_t i;
	for (i = 1; i < ACS_LEN; ++i) {
	    if (real_map[i] == 0) {
		real_map[i] = (chtype) i;
		if (real_map != fake_map) {
		    if (sp != 0)
			sp->_screen_acs_map[i] = TRUE;
		}
	    }
	}
    }
#endif

    if (acs_chars != NULL) {
	size_t i = 0;
	size_t length = strlen(acs_chars);

	while (i + 1 < length) {
	    if (acs_chars[i] != 0 && UChar(acs_chars[i]) < ACS_LEN) {
		real_map[UChar(acs_chars[i])] = UChar(acs_chars[i + 1]) | A_ALTCHARSET;
		T(("#%d real_map[%s] = %s",
		   (int) i,
		   _tracechar(UChar(acs_chars[i])),
		   _tracechtype(real_map[UChar(acs_chars[i])])));
		if (sp != 0) {
		    sp->_screen_acs_map[UChar(acs_chars[i])] = TRUE;
		}
	    }
	    i += 2;
	}
    }
#ifdef TRACE
    /* Show the equivalent mapping, noting if it does not match the
     * given attribute, whether by re-ordering or duplication.
     */
    if (USE_TRACEF(TRACE_CALLS)) {
	size_t n, m;
	char show[ACS_LEN * 2 + 1];
	for (n = 1, m = 0; n < ACS_LEN; n++) {
	    if (real_map[n] != 0) {
		show[m++] = (char) n;
		show[m++] = (char) ChCharOf(real_map[n]);
	    }
	}
	show[m] = 0;
	if (acs_chars == NULL || strcmp(acs_chars, show))
	    _tracef("%s acs_chars %s",
		    (acs_chars == NULL) ? "NULL" : "READ",
		    _nc_visbuf(acs_chars));
	_tracef("%s acs_chars %s",
		(acs_chars == NULL)
		? "NULL"
		: (strcmp(acs_chars, show)
		   ? "DIFF"
		   : "SAME"),
		_nc_visbuf(show));
	_nc_unlock_global(tracef);
    }
#endif /* TRACE */
}