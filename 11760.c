drv_print(TERMINAL_CONTROL_BLOCK * TCB, char *data, int len)
{
    SCREEN *sp;

    AssertTCB();
    SetSP();
#if NCURSES_EXT_FUNCS
    return NCURSES_SP_NAME(mcprint) (TCB->csp, data, len);
#else
    return ERR;
#endif
}