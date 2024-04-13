drv_getsize(TERMINAL_CONTROL_BLOCK * TCB, int *l, int *c)
{
    AssertTCB();
    assert(l != 0 && c != 0);
    *l = lines;
    *c = columns;
    return OK;
}