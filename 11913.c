NCURSES_SP_NAME(_nc_reset_colors) (NCURSES_SP_DCL0)
{
    int result = FALSE;

    T((T_CALLED("_nc_reset_colors(%p)"), (void *) SP_PARM));
    if (SP_PARM->_color_defs > 0)
	SP_PARM->_color_defs = -(SP_PARM->_color_defs);
    if (reset_color_pair(NCURSES_SP_ARG))
	result = TRUE;

#ifdef USE_TERM_DRIVER
    result = CallDriver(SP_PARM, td_rescolors);
#else
    if (orig_colors != 0) {
	NCURSES_PUTP2("orig_colors", orig_colors);
	result = TRUE;
    }
#endif
    returnBool(result);
}