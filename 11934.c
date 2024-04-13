_nc_real_mvcur(NCURSES_SP_DCLx
	       int yold, int xold,
	       int ynew, int xnew,
	       NCURSES_SP_OUTC myOutCh,
	       int ovw)
{
    NCURSES_CH_T oldattr;
    int code;

    TR(TRACE_CALLS | TRACE_MOVE, (T_CALLED("_nc_tinfo_mvcur(%p,%d,%d,%d,%d)"),
				  (void *) SP_PARM, yold, xold, ynew, xnew));

    if (SP_PARM == 0) {
	code = ERR;
    } else if (yold == ynew && xold == xnew) {
	code = OK;
    } else {

	/*
	 * Most work here is rounding for terminal boundaries getting the
	 * column position implied by wraparound or the lack thereof and
	 * rolling up the screen to get ynew on the screen.
	 */
	if (xnew >= screen_columns(SP_PARM)) {
	    ynew += xnew / screen_columns(SP_PARM);
	    xnew %= screen_columns(SP_PARM);
	}

	/*
	 * Force restore even if msgr is on when we're in an alternate
	 * character set -- these have a strong tendency to screw up the CR &
	 * LF used for local character motions!
	 */
	oldattr = SCREEN_ATTRS(SP_PARM);
	if ((AttrOf(oldattr) & A_ALTCHARSET)
	    || (AttrOf(oldattr) && !move_standout_mode)) {
	    TR(TRACE_CHARPUT, ("turning off (%#lx) %s before move",
			       (unsigned long) AttrOf(oldattr),
			       _traceattr(AttrOf(oldattr))));
	    VIDPUTS(SP_PARM, A_NORMAL, 0);
	}

	if (xold >= screen_columns(SP_PARM)) {

	    if (SP_PARM->_nl) {
		int l = (xold + 1) / screen_columns(SP_PARM);

		yold += l;
		if (yold >= screen_lines(SP_PARM))
		    l -= (yold - screen_lines(SP_PARM) - 1);

		if (l > 0) {
		    if (carriage_return) {
			NCURSES_PUTP2("carriage_return", carriage_return);
		    } else {
			myOutCh(NCURSES_SP_ARGx '\r');
		    }
		    xold = 0;

		    while (l > 0) {
			if (newline) {
			    NCURSES_PUTP2("newline", newline);
			} else {
			    myOutCh(NCURSES_SP_ARGx '\n');
			}
			l--;
		    }
		}
	    } else {
		/*
		 * If caller set nonl(), we cannot really use newlines to
		 * position to the next row.
		 */
		xold = -1;
		yold = -1;
	    }
	}

	if (yold > screen_lines(SP_PARM) - 1)
	    yold = screen_lines(SP_PARM) - 1;
	if (ynew > screen_lines(SP_PARM) - 1)
	    ynew = screen_lines(SP_PARM) - 1;

	/* destination location is on screen now */
	code = onscreen_mvcur(NCURSES_SP_ARGx yold, xold, ynew, xnew, ovw, myOutCh);

	/*
	 * Restore attributes if we disabled them before moving.
	 */
	if (!SameAttrOf(oldattr, SCREEN_ATTRS(SP_PARM))) {
	    TR(TRACE_CHARPUT, ("turning on (%#lx) %s after move",
			       (unsigned long) AttrOf(oldattr),
			       _traceattr(AttrOf(oldattr))));
	    VIDPUTS(SP_PARM, AttrOf(oldattr), GetPair(oldattr));
	}
    }
    returnCode(code);
}