NCURSES_SP_NAME(init_extended_pair) (NCURSES_SP_DCLx int pair, int f, int b)
{
    return _nc_init_pair(SP_PARM, pair, f, b);
}