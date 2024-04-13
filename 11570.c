NCURSES_SP_NAME(mvcur) (NCURSES_SP_DCLx int yold, int xold, int ynew,
			int xnew)
{
    return _nc_real_mvcur(NCURSES_SP_ARGx
			  yold, xold,
			  ynew, xnew,
			  NCURSES_SP_NAME(_nc_putchar),
			  FALSE);
}