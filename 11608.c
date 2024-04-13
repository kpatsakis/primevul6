enable_gpm_mouse(SCREEN *sp, bool enable)
{
    bool result;

    T((T_CALLED("enable_gpm_mouse(%d)"), enable));

    if (enable && !sp->_mouse_active) {
#ifdef HAVE_LIBDL
	if (sp->_mouse_gpm_found && !sp->_mouse_gpm_loaded) {
	    load_gpm_library(sp);
	}
#endif
	if (sp->_mouse_gpm_loaded) {
	    int code;

	    /* GPM: initialize connection to gpm server */
	    sp->_mouse_gpm_connect.eventMask = GPM_DOWN | GPM_UP;
	    sp->_mouse_gpm_connect.defaultMask =
		(unsigned short) (~(sp->_mouse_gpm_connect.eventMask | GPM_HARD));
	    sp->_mouse_gpm_connect.minMod = 0;
	    sp->_mouse_gpm_connect.maxMod =
		(unsigned short) (~((1 << KG_SHIFT) |
				    (1 << KG_SHIFTL) |
				    (1 << KG_SHIFTR)));
	    /*
	     * Note: GPM hardcodes \E[?1001s and \E[?1000h during its open.
	     * The former is recognized by wscons (SunOS), and the latter by
	     * xterm.  Those will not show up in ncurses' traces.
	     */
	    code = my_Gpm_Open(&sp->_mouse_gpm_connect, 0);
	    result = (code >= 0);

	    /*
	     * GPM can return a -2 if it is trying to do something with xterm.
	     * Ignore that, since it conflicts with our use of stdin.
	     */
	    if (code == -2) {
		my_Gpm_Close();
	    }
	} else {
	    result = FALSE;
	}
	sp->_mouse_active = result;
	T(("GPM open %s", result ? "succeeded" : "failed"));
    } else {
	if (!enable && sp->_mouse_active) {
	    /* GPM: close connection to gpm server */
	    my_Gpm_Close();
	    sp->_mouse_active = FALSE;
	    T(("GPM closed"));
	}
	result = enable;
    }
#ifdef HAVE_LIBDL
    if (!result) {
	unload_gpm_library(sp);
    }
#endif
    returnBool(result);
}