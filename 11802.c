NCURSES_PUBLIC_VAR(_nc_outchars) (void)
{
    return CURRENT_SCREEN ? CURRENT_SCREEN->_outchars : _nc_prescreen._outchars;
}