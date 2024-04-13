vid_puts(attr_t newmode,
	 NCURSES_PAIRS_T pair_arg,
	 void *opts GCC_UNUSED,
	 NCURSES_OUTC outc)
{
    SetSafeOutcWrapper(outc);
    return NCURSES_SP_NAME(vid_puts) (CURRENT_SCREEN,
				      newmode,
				      pair_arg,
				      opts,
				      _nc_outc_wrapper);
}