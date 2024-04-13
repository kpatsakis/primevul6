drv_sgmode(TERMINAL_CONTROL_BLOCK * TCB, int setFlag, TTY * buf)
{
    SCREEN *sp = TCB->csp;
    TERMINAL *_term = (TERMINAL *) TCB;
    int result = OK;

    AssertTCB();
    if (setFlag) {
	for (;;) {
	    if (SET_TTY(_term->Filedes, buf) != 0) {
		if (errno == EINTR)
		    continue;
		if (errno == ENOTTY) {
		    if (sp)
			sp->_notty = TRUE;
		}
		result = ERR;
	    }
	    break;
	}
    } else {
	for (;;) {
	    if (GET_TTY(_term->Filedes, buf) != 0) {
		if (errno == EINTR)
		    continue;
		result = ERR;
	    }
	    break;
	}
    }
    return result;
}