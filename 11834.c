TINFO_MVCUR(NCURSES_SP_DCLx int yold, int xold, int ynew, int xnew)
{
    int rc;
    rc = _nc_real_mvcur(NCURSES_SP_ARGx
			yold, xold,
			ynew, xnew,
			NCURSES_SP_NAME(_nc_outch),
			TRUE);
    if ((SP_PARM != 0) && (SP_PARM->_endwin == ewInitial))
	NCURSES_SP_NAME(_nc_flush) (NCURSES_SP_ARG);
    NCURSES_SP_NAME(_nc_flush) (NCURSES_SP_ARG);
    return rc;
}