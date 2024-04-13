PutCharLR(NCURSES_SP_DCLx const ARG_CH_T ch)
{
    if (!auto_right_margin) {
	/* we can put the char directly */
	PutAttrChar(NCURSES_SP_ARGx ch);
    } else if (enter_am_mode && exit_am_mode) {
	/* we can suppress automargin */
	NCURSES_PUTP2("exit_am_mode", exit_am_mode);

	PutAttrChar(NCURSES_SP_ARGx ch);
	SP_PARM->_curscol--;
	position_check(NCURSES_SP_ARGx
		       SP_PARM->_cursrow,
		       SP_PARM->_curscol,
		       "exit_am_mode");

	NCURSES_PUTP2("enter_am_mode", enter_am_mode);
    } else if ((enter_insert_mode && exit_insert_mode)
	       || insert_character || parm_ich) {
	GoTo(NCURSES_SP_ARGx
	     screen_lines(SP_PARM) - 1,
	     screen_columns(SP_PARM) - 2);
	PutAttrChar(NCURSES_SP_ARGx ch);
	GoTo(NCURSES_SP_ARGx
	     screen_lines(SP_PARM) - 1,
	     screen_columns(SP_PARM) - 2);
	InsStr(NCURSES_SP_ARGx
	       NewScreen(SP_PARM)->_line[screen_lines(SP_PARM) - 1].text +
	       screen_columns(SP_PARM) - 2, 1);
    }
}