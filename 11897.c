wrap_cursor(NCURSES_SP_DCL0)
{
    if (eat_newline_glitch) {
	/*
	 * xenl can manifest two different ways.  The vt100 way is that, when
	 * you'd expect the cursor to wrap, it stays hung at the right margin
	 * (on top of the character just emitted) and doesn't wrap until the
	 * *next* graphic char is emitted.  The c100 way is to ignore LF
	 * received just after an am wrap.
	 *
	 * An aggressive way to handle this would be to emit CR/LF after the
	 * char and then assume the wrap is done, you're on the first position
	 * of the next line, and the terminal out of its weird state.  Here
	 * it's safe to just tell the code that the cursor is in hyperspace and
	 * let the next mvcur() call straighten things out.
	 */
	SP_PARM->_curscol = -1;
	SP_PARM->_cursrow = -1;
    } else if (auto_right_margin) {
	SP_PARM->_curscol = 0;
	SP_PARM->_cursrow++;
	/*
	 * We've actually moved - but may have to work around problems with
	 * video attributes not working.
	 */
	if (!move_standout_mode && AttrOf(SCREEN_ATTRS(SP_PARM))) {
	    TR(TRACE_CHARPUT, ("turning off (%#lx) %s before wrapping",
			       (unsigned long) AttrOf(SCREEN_ATTRS(SP_PARM)),
			       _traceattr(AttrOf(SCREEN_ATTRS(SP_PARM)))));
	    VIDPUTS(SP_PARM, A_NORMAL, 0);
	}
    } else {
	SP_PARM->_curscol--;
    }
    position_check(NCURSES_SP_ARGx
		   SP_PARM->_cursrow,
		   SP_PARM->_curscol,
		   "wrap_cursor");
}