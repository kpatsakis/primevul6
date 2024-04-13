mcprint(char *data, int len)
{
    return NCURSES_SP_NAME(mcprint) (CURRENT_SCREEN, data, len);
}