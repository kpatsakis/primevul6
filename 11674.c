NCURSES_SP_NAME(_nc_do_xmc_glitch) (NCURSES_SP_DCLx attr_t previous)
{
    if (SP_PARM != 0) {
	attr_t chg = XMC_CHANGES(previous ^ AttrOf(SCREEN_ATTRS(SP_PARM)));

	while (chg != 0) {
	    if (chg & 1) {
		SP_PARM->_curscol += magic_cookie_glitch;
		if (SP_PARM->_curscol >= SP_PARM->_columns)
		    wrap_cursor(NCURSES_SP_ARG);
		TR(TRACE_UPDATE, ("bumped to %d,%d after cookie",
				  SP_PARM->_cursrow, SP_PARM->_curscol));
	    }
	    chg >>= 1;
	}
    }
}