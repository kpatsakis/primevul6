NCURSES_SP_NAME(_nc_mvcur_resume) (NCURSES_SP_DCL0)
/* what to do at initialization time and after each shellout */
{
    if (!SP_PARM || !IsTermInfo(SP_PARM))
	return;

    /* initialize screen for cursor access */
    if (enter_ca_mode) {
	NCURSES_PUTP2("enter_ca_mode", enter_ca_mode);
    }

    /*
     * Doing this here rather than in _nc_mvcur_wrap() ensures that
     * ncurses programs will see a reset scroll region even if a
     * program that messed with it died ungracefully.
     *
     * This also undoes the effects of terminal init strings that assume
     * they know the screen size.  This is useful when you're running
     * a vt100 emulation through xterm.
     */
    reset_scroll_region(NCURSES_SP_ARG);
    SP_PARM->_cursrow = SP_PARM->_curscol = -1;

    /* restore cursor shape */
    if (SP_PARM->_cursor != -1) {
	int cursor = SP_PARM->_cursor;
	SP_PARM->_cursor = -1;
	NCURSES_SP_NAME(curs_set) (NCURSES_SP_ARGx cursor);
    }
}