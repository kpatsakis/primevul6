GoTo(NCURSES_SP_DCLx int const row, int const col)
{
    TR(TRACE_MOVE, ("GoTo(%p, %d, %d) from (%d, %d)",
		    (void *) SP_PARM, row, col, SP_PARM->_cursrow, SP_PARM->_curscol));

    position_check(NCURSES_SP_ARGx
		   SP_PARM->_cursrow,
		   SP_PARM->_curscol, "GoTo");

    TINFO_MVCUR(NCURSES_SP_ARGx
		SP_PARM->_cursrow,
		SP_PARM->_curscol,
		row, col);
    position_check(NCURSES_SP_ARGx
		   SP_PARM->_cursrow,
		   SP_PARM->_curscol, "GoTo2");
}