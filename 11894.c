ungetmouse(MEVENT * aevent)
{
    return NCURSES_SP_NAME(ungetmouse) (CURRENT_SCREEN, aevent);
}