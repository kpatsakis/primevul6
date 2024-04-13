default_fg(SCREEN *sp)
{
#if NCURSES_EXT_FUNCS
    return (sp != 0) ? sp->_default_fg : COLOR_WHITE;
#else
    return COLOR_WHITE;
#endif
}