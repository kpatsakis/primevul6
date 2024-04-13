ClrToEOS(NCURSES_SP_DCLx NCURSES_CH_T blank)
{
    int row, col;

    row = SP_PARM->_cursrow;
    col = SP_PARM->_curscol;

    if (row < 0)
	row = 0;
    if (col < 0)
	col = 0;

    UpdateAttrs(SP_PARM, blank);
    TPUTS_TRACE("clr_eos");
    NCURSES_SP_NAME(tputs) (NCURSES_SP_ARGx
			    clr_eos,
			    screen_lines(SP_PARM) - row,
			    NCURSES_SP_NAME(_nc_outch));

    while (col < screen_columns(SP_PARM))
	CurScreen(SP_PARM)->_line[row].text[col++] = blank;

    for (row++; row < screen_lines(SP_PARM); row++) {
	for (col = 0; col < screen_columns(SP_PARM); col++)
	    CurScreen(SP_PARM)->_line[row].text[col] = blank;
    }
}