drv_conattr(TERMINAL_CONTROL_BLOCK * TCB)
{
    SCREEN *sp = TCB->csp;
    chtype attrs = A_NORMAL;

    AssertTCB();
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

    if (sp && sp->_coloron)
	attrs |= A_COLOR;

#if USE_ITALIC
    if (enter_italics_mode)
	attrs |= A_ITALIC;
#endif

    return (attrs);
}