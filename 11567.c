NCURSES_SP_NAME(vid_attr) (NCURSES_SP_DCLx
			   attr_t newmode,
			   NCURSES_PAIRS_T pair_arg,
			   void *opts)
{
    T((T_CALLED("vid_attr(%s,%d)"), _traceattr(newmode), (int) pair_arg));
    returnCode(NCURSES_SP_NAME(vid_puts) (NCURSES_SP_ARGx
					  newmode,
					  pair_arg,
					  opts,
					  NCURSES_SP_NAME(_nc_putchar)));
}