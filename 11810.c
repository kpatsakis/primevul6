ClearScreen(NCURSES_SP_DCLx NCURSES_CH_T blank)
{
    int i, j;
    bool fast_clear = (clear_screen || clr_eos || clr_eol);

    TR(TRACE_UPDATE, ("ClearScreen() called"));

#if NCURSES_EXT_FUNCS
    if (SP_PARM->_coloron
	&& !SP_PARM->_default_color) {
	NCURSES_SP_NAME(_nc_do_color) (NCURSES_SP_ARGx
				       (short) GET_SCREEN_PAIR(SP_PARM),
				       0,
				       FALSE,
				       NCURSES_SP_NAME(_nc_outch));
	if (!back_color_erase) {
	    fast_clear = FALSE;
	}
    }
#endif

    if (fast_clear) {
	if (clear_screen) {
	    UpdateAttrs(SP_PARM, blank);
	    NCURSES_PUTP2("clear_screen", clear_screen);
	    SP_PARM->_cursrow = SP_PARM->_curscol = 0;
	    position_check(NCURSES_SP_ARGx
			   SP_PARM->_cursrow,
			   SP_PARM->_curscol,
			   "ClearScreen");
	} else if (clr_eos) {
	    SP_PARM->_cursrow = SP_PARM->_curscol = -1;
	    GoTo(NCURSES_SP_ARGx 0, 0);
	    UpdateAttrs(SP_PARM, blank);
	    TPUTS_TRACE("clr_eos");
	    NCURSES_SP_NAME(tputs) (NCURSES_SP_ARGx
				    clr_eos,
				    screen_lines(SP_PARM),
				    NCURSES_SP_NAME(_nc_outch));
	} else if (clr_eol) {
	    SP_PARM->_cursrow = SP_PARM->_curscol = -1;
	    UpdateAttrs(SP_PARM, blank);
	    for (i = 0; i < screen_lines(SP_PARM); i++) {
		GoTo(NCURSES_SP_ARGx i, 0);
		NCURSES_PUTP2("clr_eol", clr_eol);
	    }
	    GoTo(NCURSES_SP_ARGx 0, 0);
	}
    } else {
	UpdateAttrs(SP_PARM, blank);
	for (i = 0; i < screen_lines(SP_PARM); i++) {
	    GoTo(NCURSES_SP_ARGx i, 0);
	    for (j = 0; j < screen_columns(SP_PARM); j++)
		PutChar(NCURSES_SP_ARGx CHREF(blank));
	}
	GoTo(NCURSES_SP_ARGx 0, 0);
    }

    for (i = 0; i < screen_lines(SP_PARM); i++) {
	for (j = 0; j < screen_columns(SP_PARM); j++)
	    CurScreen(SP_PARM)->_line[i].text[j] = blank;
    }

    TR(TRACE_UPDATE, ("screen cleared"));
}