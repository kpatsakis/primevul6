vidputs(chtype newmode, NCURSES_OUTC outc)
{
    SetSafeOutcWrapper(outc);
    return NCURSES_SP_NAME(vidputs) (CURRENT_SCREEN,
				     newmode,
				     _nc_outc_wrapper);
}