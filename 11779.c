NCURSES_SP_NAME(termattrs) (NCURSES_SP_DCL0)
{
    chtype attrs = A_NORMAL;

    T((T_CALLED("termattrs(%p)"), (void *) SP_PARM));

    if (HasTerminal(SP_PARM)) {
#ifdef USE_TERM_DRIVER
	attrs = CallDriver(SP_PARM, td_conattr);
#else /* ! USE_TERM_DRIVER */

	if (enter_alt_charset_mode)
	    attrs |= A_ALTCHARSET;

	if (enter_blink_mode)
	    attrs |= A_BLINK;

	if (enter_bold_mode)
	    attrs |= A_BOLD;

	if (enter_dim_mode)
	    attrs |= A_DIM;

	if (enter_reverse_mode)
	    attrs |= A_REVERSE;

	if (enter_standout_mode)
	    attrs |= A_STANDOUT;

	if (enter_protected_mode)
	    attrs |= A_PROTECT;

	if (enter_secure_mode)
	    attrs |= A_INVIS;

	if (enter_underline_mode)
	    attrs |= A_UNDERLINE;

	if (SP_PARM->_coloron)
	    attrs |= A_COLOR;

#if USE_ITALIC
	if (enter_italics_mode)
	    attrs |= A_ITALIC;
#endif

#endif /* USE_TERM_DRIVER */
    }
    returnChtype(attrs);
}