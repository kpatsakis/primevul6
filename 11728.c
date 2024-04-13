NCURSES_SP_NAME(init_pair) (NCURSES_SP_DCLx
			    NCURSES_PAIRS_T pair,
			    NCURSES_COLOR_T f,
			    NCURSES_COLOR_T b)
{
    return _nc_init_pair(SP_PARM, pair, f, b);
}