drv_setsize(TERMINAL_CONTROL_BLOCK * TCB, int l, int c)
{
    AssertTCB();
    lines = (short) l;
    columns = (short) c;
    return OK;
}