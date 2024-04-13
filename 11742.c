init_xterm_mouse(SCREEN *sp)
{
    sp->_mouse_type = M_XTERM;
    sp->_mouse_xtermcap = NCURSES_SP_NAME(tigetstr) (NCURSES_SP_ARGx "XM");
    if (!VALID_STRING(sp->_mouse_xtermcap))
	sp->_mouse_xtermcap = "\033[?1000%?%p1%{1}%=%th%el%;";
}