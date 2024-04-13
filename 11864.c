vidattr(chtype newmode)
{
    return NCURSES_SP_NAME(vidattr) (CURRENT_SCREEN, newmode);
}