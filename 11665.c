drv_doupdate(TERMINAL_CONTROL_BLOCK * TCB)
{
    AssertTCB();
    return TINFO_DOUPDATE(TCB->csp);
}