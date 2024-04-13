drv_kpad(TERMINAL_CONTROL_BLOCK * TCB, int flag)
{
    int ret = ERR;
    SCREEN *sp;

    AssertTCB();

    sp = TCB->csp;

    if (sp) {
	if (flag) {
	    (void) __nc_putp_flush(sp, "keypad_xmit", keypad_xmit);
	} else if (!flag && keypad_local) {
	    (void) __nc_putp_flush(sp, "keypad_local", keypad_local);
	}
	if (flag && !sp->_tried) {
	    _nc_init_keytry(sp);
	    sp->_tried = TRUE;
	}
	ret = OK;
    }

    return ret;
}