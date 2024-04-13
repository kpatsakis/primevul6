reset_color_pair(NCURSES_SP_DCL0)
{
#ifdef USE_TERM_DRIVER
    return CallDriver(SP_PARM, td_rescol);
#else
    bool result = FALSE;

    (void) SP_PARM;
    if (orig_pair != 0) {
	(void) NCURSES_PUTP2("orig_pair", orig_pair);
	result = TRUE;
    }
    return result;
#endif
}