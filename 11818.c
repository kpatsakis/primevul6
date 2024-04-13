can_clear_with(NCURSES_SP_DCLx ARG_CH_T ch)
{
    if (!back_color_erase && SP_PARM->_coloron) {
#if NCURSES_EXT_FUNCS
	int pair;

	if (!SP_PARM->_default_color)
	    return FALSE;
	if (!(isDefaultColor(SP_PARM->_default_fg) &&
	      isDefaultColor(SP_PARM->_default_bg)))
	    return FALSE;
	if ((pair = GetPair(CHDEREF(ch))) != 0) {
	    NCURSES_COLOR_T fg, bg;
	    if (NCURSES_SP_NAME(pair_content) (NCURSES_SP_ARGx
					       (short) pair,
					       &fg, &bg) == ERR
		|| !(isDefaultColor(fg) && isDefaultColor(bg))) {
		return FALSE;
	    }
	}
#else
	if (AttrOfD(ch) & A_COLOR)
	    return FALSE;
#endif
    }
    return (ISBLANK(CHDEREF(ch)) &&
	    (AttrOfD(ch) & ~(NONBLANK_ATTR | A_COLOR)) == BLANK_ATTR);
}