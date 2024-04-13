NCURSES_SP_NAME(_nc_screen_resume) (NCURSES_SP_DCL0)
{
    assert(SP_PARM);

    /* make sure terminal is in a sane known state */
    SetAttr(SCREEN_ATTRS(SP_PARM), A_NORMAL);
    NewScreen(SP_PARM)->_clear = TRUE;

    /* reset color pairs and definitions */
    if (SP_PARM->_coloron || SP_PARM->_color_defs)
	NCURSES_SP_NAME(_nc_reset_colors) (NCURSES_SP_ARG);

    /* restore user-defined colors, if any */
    if (SP_PARM->_color_defs < 0 && !SP_PARM->_direct_color.value) {
	int n;
	SP_PARM->_color_defs = -(SP_PARM->_color_defs);
	for (n = 0; n < SP_PARM->_color_defs; ++n) {
	    if (SP_PARM->_color_table[n].init) {
		_nc_init_color(SP_PARM,
			       n,
			       SP_PARM->_color_table[n].r,
			       SP_PARM->_color_table[n].g,
			       SP_PARM->_color_table[n].b);
	    }
	}
    }

    if (exit_attribute_mode)
	NCURSES_PUTP2("exit_attribute_mode", exit_attribute_mode);
    else {
	/* turn off attributes */
	if (exit_alt_charset_mode)
	    NCURSES_PUTP2("exit_alt_charset_mode", exit_alt_charset_mode);
	if (exit_standout_mode)
	    NCURSES_PUTP2("exit_standout_mode", exit_standout_mode);
	if (exit_underline_mode)
	    NCURSES_PUTP2("exit_underline_mode", exit_underline_mode);
    }
    if (exit_insert_mode)
	NCURSES_PUTP2("exit_insert_mode", exit_insert_mode);
    if (enter_am_mode && exit_am_mode) {
	if (auto_right_margin) {
	    NCURSES_PUTP2("enter_am_mode", enter_am_mode);
	} else {
	    NCURSES_PUTP2("exit_am_mode", exit_am_mode);
	}
    }
}