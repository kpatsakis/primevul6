drv_screen_init(SCREEN *sp)
{
    TERMINAL_CONTROL_BLOCK *TCB = TCBOf(sp);

    AssertTCB();

    /*
     * Check for mismatched graphic-rendition capabilities.  Most SVr4
     * terminfo trees contain entries that have rmul or rmso equated to
     * sgr0 (Solaris curses copes with those entries).  We do this only
     * for curses, since many termcap applications assume that
     * smso/rmso and smul/rmul are paired, and will not function
     * properly if we remove rmso or rmul.  Curses applications
     * shouldn't be looking at this detail.
     */
    sp->_use_rmso = SGR0_TEST(exit_standout_mode);
    sp->_use_rmul = SGR0_TEST(exit_underline_mode);

    /*
     * Check whether we can optimize scrolling under dumb terminals in
     * case we do not have any of these capabilities, scrolling
     * optimization will be useless.
     */
    sp->_scrolling = ((scroll_forward && scroll_reverse) ||
		      ((parm_rindex ||
			parm_insert_line ||
			insert_line) &&
		       (parm_index ||
			parm_delete_line ||
			delete_line)));

    NCURSES_SP_NAME(baudrate) (sp);

    NCURSES_SP_NAME(_nc_mvcur_init) (sp);
    /* initialize terminal to a sane state */
    NCURSES_SP_NAME(_nc_screen_init) (sp);
}