NCURSES_PUBLIC_VAR(COLORS) (void)
{
    return CURRENT_SCREEN ? CURRENT_SCREEN->_color_count : -1;
}