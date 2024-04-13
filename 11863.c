_nc_pair_content(SCREEN *sp, int pair, int *f, int *b)
{
    int result;

    T((T_CALLED("pair_content(%p,%d,%p,%p)"),
       (void *) sp,
       (int) pair,
       (void *) f,
       (void *) b));

    if (!ValidPair(sp, pair)) {
	result = ERR;
    } else {
	int fg;
	int bg;

	ReservePairs(sp, pair);
	fg = FORE_OF(sp->_color_pairs[pair]);
	bg = BACK_OF(sp->_color_pairs[pair]);
#if NCURSES_EXT_FUNCS
	if (isDefaultColor(fg))
	    fg = -1;
	if (isDefaultColor(bg))
	    bg = -1;
#endif

	if (f)
	    *f = fg;
	if (b)
	    *b = bg;

	TR(TRACE_ATTRS, ("...pair_content(%p,%d,%d,%d)",
			 (void *) sp,
			 (int) pair,
			 (int) fg, (int) bg));
	result = OK;
    }
    returnCode(result);
}