mvcur(int yold, int xold, int ynew, int xnew)
{
    return NCURSES_SP_NAME(mvcur) (CURRENT_SCREEN, yold, xold, ynew, xnew);
}