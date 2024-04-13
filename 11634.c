drv_mvcur(TERMINAL_CONTROL_BLOCK * TCB, int yold, int xold, int ynew, int xnew)
{
    SCREEN *sp = TCB->csp;
    AssertTCB();
    return NCURSES_SP_NAME(_nc_mvcur) (sp, yold, xold, ynew, xnew);
}