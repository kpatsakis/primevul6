drv_defaultcolors(TERMINAL_CONTROL_BLOCK * TCB, int fg, int bg)
{
    SCREEN *sp;
    int code = ERR;

    AssertTCB();
    SetSP();

    if (sp != 0 && orig_pair && orig_colors && (initialize_pair != 0)) {
#if NCURSES_EXT_FUNCS
	sp->_default_color = isDefaultColor(fg) || isDefaultColor(bg);
	sp->_has_sgr_39_49 = (NCURSES_SP_NAME(tigetflag) (NCURSES_SP_ARGx
							  "AX")
			      == TRUE);
	sp->_default_fg = isDefaultColor(fg) ? COLOR_DEFAULT : fg;
	sp->_default_bg = isDefaultColor(bg) ? COLOR_DEFAULT : bg;
	if (sp->_color_pairs != 0) {
	    bool save = sp->_default_color;
	    sp->_default_color = TRUE;
	    NCURSES_SP_NAME(init_pair) (NCURSES_SP_ARGx
					0,
					(short)fg,
					(short)bg);
	    sp->_default_color = save;
	}
#endif
	code = OK;
    }
    return (code);
}