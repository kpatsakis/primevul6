drv_initmouse(TERMINAL_CONTROL_BLOCK * TCB)
{
    SCREEN *sp;

    AssertTCB();
    SetSP();

    /* we know how to recognize mouse events under "xterm" */
    if (sp != 0) {
	if (NonEmpty(key_mouse)) {
	    init_xterm_mouse(sp);
	} else if (strstr(SP_TERMTYPE term_names, "xterm") != 0) {
	    if (_nc_add_to_try(&(sp->_keytry), xterm_kmous, KEY_MOUSE) == OK)
		init_xterm_mouse(sp);
	}
    }
}