NCURSES_SP_NAME(_nc_mvcur_wrap) (NCURSES_SP_DCL0)
/* wrap up cursor-addressing mode */
{
    /* leave cursor at screen bottom */
    TINFO_MVCUR(NCURSES_SP_ARGx -1, -1, screen_lines(SP_PARM) - 1, 0);

    if (!SP_PARM || !IsTermInfo(SP_PARM))
	return;

    /* set cursor to normal mode */
    if (SP_PARM->_cursor != -1) {
	int cursor = SP_PARM->_cursor;
	NCURSES_SP_NAME(curs_set) (NCURSES_SP_ARGx 1);
	SP_PARM->_cursor = cursor;
    }

    if (exit_ca_mode) {
	NCURSES_PUTP2("exit_ca_mode", exit_ca_mode);
    }
    /*
     * Reset terminal's tab counter.  There's a long-time bug that
     * if you exit a "curses" program such as vi or more, tab
     * forward, and then backspace, the cursor doesn't go to the
     * right place.  The problem is that the kernel counts the
     * escape sequences that reset things as column positions.
     * Utter a \r to reset this invisibly.
     */
    NCURSES_SP_NAME(_nc_outch) (NCURSES_SP_ARGx '\r');
}