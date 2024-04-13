ClrToEOL(NCURSES_SP_DCLx NCURSES_CH_T blank, int needclear)
{
    if (CurScreen(SP_PARM) != 0
	&& SP_PARM->_cursrow >= 0) {
	int j;

	for (j = SP_PARM->_curscol; j < screen_columns(SP_PARM); j++) {
	    if (j >= 0) {
		NCURSES_CH_T *cp =
		&(CurScreen(SP_PARM)->_line[SP_PARM->_cursrow].text[j]);

		if (!CharEq(*cp, blank)) {
		    *cp = blank;
		    needclear = TRUE;
		}
	    }
	}
    }

    if (needclear) {
	UpdateAttrs(SP_PARM, blank);
	if (clr_eol && SP_PARM->_el_cost <= (screen_columns(SP_PARM) - SP_PARM->_curscol)) {
	    NCURSES_PUTP2("clr_eol", clr_eol);
	} else {
	    int count = (screen_columns(SP_PARM) - SP_PARM->_curscol);
	    while (count-- > 0)
		PutChar(NCURSES_SP_ARGx CHREF(blank));
	}
    }
}