default_bg(SCREEN *sp)
{
#if NCURSES_EXT_FUNCS
    return sp != 0 ? sp->_default_bg : COLOR_BLACK;
#else
    return COLOR_BLACK;
#endif
}