NCURSES_PUBLIC_VAR(COLORS) (void)
{
    return SP ? SP->_color_count : -1;
}