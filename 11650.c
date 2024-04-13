drv_kyExist(TERMINAL_CONTROL_BLOCK * TCB, int key)
{
    bool res = FALSE;

    AssertTCB();
    if (TCB->csp)
	res = TINFO_HAS_KEY(TCB->csp, key) == 0 ? FALSE : TRUE;

    return res;
}