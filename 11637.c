NCURSES_SP_NAME(start_color) (NCURSES_SP_DCL0)
{
    int result = ERR;
    int maxpairs = 0, maxcolors = 0;

    T((T_CALLED("start_color(%p)"), (void *) SP_PARM));

    if (SP_PARM == 0) {
	result = ERR;
    } else if (SP_PARM->_coloron) {
	result = OK;
    } else {
	maxpairs = MaxPairs;
	maxcolors = MaxColors;
	if (reset_color_pair(NCURSES_SP_ARG) != TRUE) {
	    set_foreground_color(NCURSES_SP_ARGx
				 default_fg(NCURSES_SP_ARG),
				 NCURSES_SP_NAME(_nc_outch));
	    set_background_color(NCURSES_SP_ARGx
				 default_bg(NCURSES_SP_ARG),
				 NCURSES_SP_NAME(_nc_outch));
	}
#if !NCURSES_EXT_COLORS
	/*
	 * Without ext-colors, we cannot represent more than 256 color pairs.
	 */
	if (maxpairs > 256)
	    maxpairs = 256;
#endif

	if (maxpairs > 0 && maxcolors > 0) {
	    SP_PARM->_pair_limit = maxpairs;

#if NCURSES_EXT_FUNCS
	    /*
	     * If using default colors, allocate extra space in table to
	     * allow for default-color as a component of a color-pair.
	     */
	    SP_PARM->_pair_limit += (1 + (2 * maxcolors));
#if !NCURSES_EXT_COLORS
	    SP_PARM->_pair_limit = limit_PAIRS(SP_PARM->_pair_limit);
#endif
#endif /* NCURSES_EXT_FUNCS */
	    SP_PARM->_pair_count = maxpairs;
	    SP_PARM->_color_count = maxcolors;
#if !USE_REENTRANT
	    COLOR_PAIRS = maxpairs;
	    COLORS = maxcolors;
#endif

	    ReservePairs(SP_PARM, 16);
	    if (SP_PARM->_color_pairs != 0) {
		if (init_direct_colors(NCURSES_SP_ARG)) {
		    result = OK;
		} else {
		    SP_PARM->_color_table = TYPE_CALLOC(color_t, maxcolors);
		    if (SP_PARM->_color_table != 0) {
			MakeColorPair(SP_PARM->_color_pairs[0],
				      default_fg(NCURSES_SP_ARG),
				      default_bg(NCURSES_SP_ARG));
			init_color_table(NCURSES_SP_ARG);

			result = OK;
		    }
		}
		if (result == OK) {
		    T(("started color: COLORS = %d, COLOR_PAIRS = %d",
		       COLORS, COLOR_PAIRS));

		    SP_PARM->_coloron = 1;
		} else if (SP_PARM->_color_pairs != 0) {
		    FreeAndNull(SP_PARM->_color_pairs);
		}
	    }
	} else {
	    result = OK;
	}
    }
    returnCode(result);
}