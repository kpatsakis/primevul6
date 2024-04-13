mouseinterval(int maxclick)
{
    return NCURSES_SP_NAME(mouseinterval) (CURRENT_SCREEN, maxclick);
}