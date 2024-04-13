NCURSES_SP_NAME(pair_content) (NCURSES_SP_DCLx
			       NCURSES_PAIRS_T pair,
			       NCURSES_COLOR_T *f,
			       NCURSES_COLOR_T *b)
{
    int my_f, my_b;
    int rc = _nc_pair_content(SP_PARM, pair, &my_f, &my_b);
    if (rc == OK) {
	*f = limit_COLOR(my_f);
	*b = limit_COLOR(my_b);
    }
    return rc;
}