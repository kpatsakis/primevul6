NCURSES_PUBLIC_VAR(COLOR_PAIRS) (void)
{
    return SP ? SP->_pair_count : -1;
}