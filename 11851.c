mouse_activate(SCREEN *sp, int on)
{
    if (!on && !sp->_mouse_initialized)
	return;

    if (!_nc_mouse_init(sp))
	return;

    if (on) {
	sp->_mouse_bstate = 0;
	switch (sp->_mouse_type) {
	case M_XTERM:
#if NCURSES_EXT_FUNCS
	    NCURSES_SP_NAME(keyok) (NCURSES_SP_ARGx KEY_MOUSE, on);
#endif
	    TPUTS_TRACE("xterm mouse initialization");
	    enable_xterm_mouse(sp, 1);
	    break;
#if USE_GPM_SUPPORT
	case M_GPM:
	    if (enable_gpm_mouse(sp, TRUE)) {
		sp->_mouse_fd = *(my_gpm_fd);
		T(("GPM mouse_fd %d", sp->_mouse_fd));
	    }
	    break;
#endif
#if USE_SYSMOUSE
	case M_SYSMOUSE:
	    signal(SIGUSR2, handle_sysmouse);
	    sp->_mouse_active = TRUE;
	    break;
#endif
#ifdef USE_TERM_DRIVER
	case M_TERM_DRIVER:
	    sp->_mouse_active = TRUE;
	    break;
#endif
	case M_NONE:
	    return;
	}
	/* Make runtime binding to cut down on object size of applications that
	 * do not use the mouse (e.g., 'clear').
	 */
	sp->_mouse_event = _nc_mouse_event;
	sp->_mouse_inline = _nc_mouse_inline;
	sp->_mouse_parse = _nc_mouse_parse;
	sp->_mouse_resume = _nc_mouse_resume;
	sp->_mouse_wrap = _nc_mouse_wrap;
    } else {

	switch (sp->_mouse_type) {
	case M_XTERM:
	    TPUTS_TRACE("xterm mouse deinitialization");
	    enable_xterm_mouse(sp, 0);
	    break;
#if USE_GPM_SUPPORT
	case M_GPM:
	    enable_gpm_mouse(sp, FALSE);
	    break;
#endif
#if USE_SYSMOUSE
	case M_SYSMOUSE:
	    signal(SIGUSR2, SIG_IGN);
	    sp->_mouse_active = FALSE;
	    break;
#endif
#ifdef USE_TERM_DRIVER
	case M_TERM_DRIVER:
	    sp->_mouse_active = FALSE;
	    break;
#endif
	case M_NONE:
	    return;
	}
    }
    NCURSES_SP_NAME(_nc_flush) (NCURSES_SP_ARG);
}