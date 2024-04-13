getmouse(MEVENT * aevent)
{
    return NCURSES_SP_NAME(getmouse) (CURRENT_SCREEN, aevent);
}