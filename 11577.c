NCURSES_SP_NAME(_nc_screen_wrap) (NCURSES_SP_DCL0)
{
    if (SP_PARM != 0) {

	UpdateAttrs(SP_PARM, normal);
#if NCURSES_EXT_FUNCS
	if (SP_PARM->_coloron
	    && !SP_PARM->_default_color) {
	    static const NCURSES_CH_T blank = NewChar(BLANK_TEXT);
	    SP_PARM->_default_color = TRUE;
	    NCURSES_SP_NAME(_nc_do_color) (NCURSES_SP_ARGx
					   -1,
					   0,
					   FALSE,
					   NCURSES_SP_NAME(_nc_outch));
	    SP_PARM->_default_color = FALSE;

	    TINFO_MVCUR(NCURSES_SP_ARGx
			SP_PARM->_cursrow,
			SP_PARM->_curscol,
			screen_lines(SP_PARM) - 1,
			0);

	    ClrToEOL(NCURSES_SP_ARGx blank, TRUE);
	}
#endif
	if (SP_PARM->_color_defs) {
	    NCURSES_SP_NAME(_nc_reset_colors) (NCURSES_SP_ARG);
	}
    }
}