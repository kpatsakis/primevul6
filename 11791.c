NCURSES_SP_NAME(term_attrs) (NCURSES_SP_DCL0)
{
    attr_t attrs = 0;

    T((T_CALLED("term_attrs()")));
    if (SP_PARM) {
	attrs = NCURSES_SP_NAME(termattrs) (NCURSES_SP_ARG);

#if USE_WIDEC_SUPPORT && defined(enter_horizontal_hl_mode)
	/* these are only supported for wide-character mode */
	if (enter_horizontal_hl_mode)
	    attrs |= WA_HORIZONTAL;
	if (enter_left_hl_mode)
	    attrs |= WA_LEFT;
	if (enter_low_hl_mode)
	    attrs |= WA_LOW;
	if (enter_right_hl_mode)
	    attrs |= WA_RIGHT;
	if (enter_top_hl_mode)
	    attrs |= WA_TOP;
	if (enter_vertical_hl_mode)
	    attrs |= WA_VERTICAL;
#endif
    }

    returnAttr(attrs);
}