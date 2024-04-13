PutChar(NCURSES_SP_DCLx const ARG_CH_T ch)
/* insert character, handling automargin stuff */
{
    if (SP_PARM->_cursrow == screen_lines(SP_PARM) - 1 &&
	SP_PARM->_curscol == screen_columns(SP_PARM) - 1) {
	PutCharLR(NCURSES_SP_ARGx ch);
    } else {
	PutAttrChar(NCURSES_SP_ARGx ch);
    }

    if (SP_PARM->_curscol >= screen_columns(SP_PARM))
	wrap_cursor(NCURSES_SP_ARG);

    position_check(NCURSES_SP_ARGx
		   SP_PARM->_cursrow,
		   SP_PARM->_curscol, "PutChar");
}