_nc_do_color(int old_pair, int pair, int reverse, NCURSES_OUTC outc)
{
    SetSafeOutcWrapper(outc);
    NCURSES_SP_NAME(_nc_do_color) (CURRENT_SCREEN,
				   old_pair,
				   pair,
				   reverse,
				   _nc_outc_wrapper);
}