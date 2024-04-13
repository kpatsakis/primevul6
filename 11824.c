NCURSES_SP_NAME(extended_pair_content) (NCURSES_SP_DCLx
					int pair,
					int *f, int *b)
{
    return _nc_pair_content(SP_PARM, pair, f, b);
}