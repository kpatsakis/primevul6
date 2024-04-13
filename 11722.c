drv_cursorSet(TERMINAL_CONTROL_BLOCK * TCB, int vis)
{
    SCREEN *sp;
    int code = ERR;

    AssertTCB();
    SetSP();

    T((T_CALLED("tinfo:drv_cursorSet(%p,%d)"), (void *) SP_PARM, vis));

    if (SP_PARM != 0 && IsTermInfo(SP_PARM)) {
	switch (vis) {
	case 2:
	    code = NCURSES_PUTP2_FLUSH("cursor_visible", cursor_visible);
	    break;
	case 1:
	    code = NCURSES_PUTP2_FLUSH("cursor_normal", cursor_normal);
	    break;
	case 0:
	    code = NCURSES_PUTP2_FLUSH("cursor_invisible", cursor_invisible);
	    break;
	}
    } else {
	code = ERR;
    }
    returnCode(code);
}