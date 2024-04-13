normalized_cost(NCURSES_SP_DCLx const char *const cap, int affcnt)
/* compute the effective character-count for an operation (round up) */
{
    int cost = NCURSES_SP_NAME(_nc_msec_cost) (NCURSES_SP_ARGx cap, affcnt);
    if (cost != INFINITY)
	cost = (cost + SP_PARM->_char_padding - 1) / SP_PARM->_char_padding;
    return cost;
}